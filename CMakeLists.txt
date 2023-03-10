cmake_minimum_required(VERSION 3.5)

project(cpptest LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# add headers, SPIRV-Reflect and shaderc from Vulkan SDK
set(VULKAN_SDK_PATH "$ENV{VULKAN_SDK}")
set(VULKAN_INCLUDE_PATH "${VULKAN_SDK_PATH}/include")
set(VULKAN_LIBRARY_PATH "${VULKAN_SDK_PATH}/lib")
set(ENV{PKG_CONFIG_PATH} "${VULKAN_LIBRARY_PATH}/pkgconfig")
set(VULKAN_LIBRARIES "vulkan")
message(STATUS "VULKAN_SDK_PATH: ${VULKAN_SDK_PATH}")
find_package(PkgConfig REQUIRED)
pkg_search_module(shaderc REQUIRED shaderc_combined)
if (NOT shaderc_FOUND)
    message(FATAL_ERROR "shaderc not found")
endif ()
list(APPEND shaderc_LIBRARIES stdc++) # TODO: Why it stopped linking correctly?

# add external library glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/glfw)

# add external library uthash
set(UTHASH_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/uthash/src")

# add external library xxHash
set(XXHASH_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/xxHash")
set(XXHASH_ADDITIONAL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/xxHash/xxhash.c")

# add external library cgltf
set(CGLTF_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cgltf")

# add external library cglm
set(CGLM_LIBRARIES "cglm")
add_subdirectory(thirdparty/cglm)

# add external library stb
set(STB_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/stb")

# add external library meshoptimizer
set(MESHOPTIMIZER_LIBRARIES "meshoptimizer")
add_subdirectory(thirdparty/meshoptimizer)

message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")

# add external library sqlite
pkg_check_modules(SQLITE3 REQUIRED IMPORTED_TARGET sqlite3)

# prepare info about external libraries
set(EXTERNAL_INCLUDE_PATHS ${VULKAN_INCLUDE_PATH} ${shaderc_INCLUDE_DIRS} ${GREATEST_INCLUDE_PATH} ${XXHASH_INCLUDE_PATH}
        ${UTHASH_INCLUDE_PATH} ${GTK4_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIRS} ${CGLTF_INCLUDE_PATH}
        ${STB_INCLUDE_PATH})
set(EXTERNAL_ADDITIONAL_SOURCES ${XXHASH_ADDITIONAL_SOURCES})
set(EXTERNAL_LIBRARY_PATHS ${VULKAN_LIBRARY_PATH} ${shaderc_LIBRARY_DIRS} ${GTK4_LIBRARY_DIRS} ${SQLITE3_LIBRARY_DIRS})
set(EXTERNAL_LIBRARIES glfw ${VULKAN_LIBRARIES} ${shaderc_LIBRARIES} ${GTK4_LIBRARIES}
        ${SQLITE3_LIBRARIES} ${CGLM_LIBRARIES} ${MESHOPTIMIZER_LIBRARIES})

# find python
set(SCRIPTS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/scripts")
find_package(Python3 COMPONENTS Interpreter Development)
set(PYTHON_VENV_PATH ${CMAKE_CURRENT_SOURCE_DIR}/venv)
if (NOT Python3_EXECUTABLE)
    message(FATAL_ERROR "Could not find `python3` in PATH")
endif ()
message(STATUS "Python3_EXECUTABLE: ${Python3_EXECUTABLE}")
execute_process(COMMAND "${Python3_EXECUTABLE}" -m venv "${PYTHON_VENV_PATH}")
set(ENV{VIRTUAL_ENV} "${PYTHON_VENV_PATH}")
set(Python3_FIND_VIRTUALENV FIRST)
unset(Python3_EXECUTABLE)
find_package(Python3 COMPONENTS Interpreter Development)
if (NOT Python3_EXECUTABLE)
    message(FATAL_ERROR "Could not find `python3` in venv")
endif ()
get_filename_component(PIP3_EXECUTABLE ${Python3_EXECUTABLE} DIRECTORY)
set(PIP3_EXECUTABLE "${PIP3_EXECUTABLE}/pip3")
message(STATUS "PIP3_EXECUTABLE: ${PIP3_EXECUTABLE}")
execute_process(COMMAND ${PIP3_EXECUTABLE} install -r ${SCRIPTS_PATH}/requirements.txt)

# define source files
set(C_SRC_DIR "src")
file(GLOB_RECURSE C_SOURCES "${C_SRC_DIR}/*.c") # without bin/ and codegen/
file(GLOB_RECURSE C_BIN_SOURCES "${C_SRC_DIR}/bin/*.c")
file(GLOB_RECURSE C_CODEGEN_SOURCES "${C_SRC_DIR}/codegen/*.c")
foreach (C_BIN_SOURCE_PATH ${C_BIN_SOURCES})
    list(REMOVE_ITEM C_SOURCES "${C_BIN_SOURCE_PATH}")
endforeach ()
foreach (C_CODEGEN_SOURCE_PATH ${C_CODEGEN_SOURCES})
    list(REMOVE_ITEM C_SOURCES "${C_CODEGEN_SOURCE_PATH}")
endforeach ()

file(GLOB_RECURSE C_HEADERS "${C_SRC_DIR}/*.h")
file(GLOB_RECURSE C_CODEGEN_HEADERS "${C_SRC_DIR}/codegen/*.h")
foreach (C_CODEGEN_HEADER_PATH ${C_CODEGEN_HEADERS})
    list(REMOVE_ITEM C_HEADERS "${C_CODEGEN_HEADER_PATH}")
endforeach ()

# add python codegen
file(GLOB_RECURSE PYTHON_CODEGEN_SCRIPTS
        "${SCRIPTS_PATH}/codegen*.py"
        "${SCRIPTS_PATH}/utils.py"
        "${SCRIPTS_PATH}/config.txt")
add_custom_command(
        OUTPUT ${C_CODEGEN_HEADERS}
        DEPENDS ${PYTHON_CODEGEN_SCRIPTS} ${C_HEADERS}
        COMMAND ${CMAKE_COMMAND} -E env "VULKAN_SDK_PATH=${VULKAN_SDK_PATH}" ${Python3_EXECUTABLE} ${SCRIPTS_PATH}/codegen.py
        COMMENT "Python codegen for headers."
        VERBATIM)


# copy unprocessed assets to directory with executables
file(GLOB_RECURSE C_PRECOMPILED_HEADERS "${C_SRC_DIR}/core/thirdparty.h")
file(GLOB ASSETS_FILES_DIR "assets")
file(GLOB ASSETS_SHADER_DIR "${ASSETS_FILES_DIR}/shaders")
file(GLOB ASSETS_GLTF_DIR "${ASSETS_FILES_DIR}/gltf")
file(GLOB ASSETS_CUBEMAP_DIR "${ASSETS_FILES_DIR}/cubemap")
file(GLOB_RECURSE ASSETS_SHADER_FILES "${ASSETS_SHADER_DIR}/*.glsl")
file(GLOB_RECURSE ASSETS_GLTF_FILES "${ASSETS_GLTF_DIR}/*.*")
file(GLOB_RECURSE ASSETS_CUBEMAP_FILES "${ASSETS_CUBEMAP_DIR}/*.*")
add_custom_target(copy_assets ALL DEPENDS ${ASSETS_GLTF_FILES} ${ASSETS_SHADER_FILES} ${ASSETS_CUBEMAP_FILES})
add_custom_command(
        TARGET copy_assets PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_FILES_DIR} ${CMAKE_CURRENT_BINARY_DIR}/assets
        COMMENT "Copy assets.")

# run asset pipeline
file(GLOB_RECURSE C_BIN_ASSET_PIPELINE_SOURCE "${C_SRC_DIR}/bin/asset_pipeline.c")
file(GLOB_RECURSE PYTHON_ASSET_SCRIPTS "${SCRIPTS_PATH}/asset_*.py")

set(ASSET_PIPELINE_STAMP_DIR "stamps")
set(ASSET_PIPELINE_STAMP ${ASSET_PIPELINE_STAMP_DIR}/"asset_pipeline")
add_custom_command(
        OUTPUT asset_pipeline.stamp
        DEPENDS ${ASSETS_GLTF_FILES} ${ASSETS_CUBEMAP_FILES} ${C_BIN_ASSET_PIPELINE_SOURCE} ${PYTHON_ASSET_SCRIPTS}
        COMMAND ${Python3_EXECUTABLE} ${SCRIPTS_PATH}/asset_pipeline.py ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E touch asset_pipeline.stamp
        COMMAND ${CMAKE_COMMAND} -E make_directory ${ASSET_PIPELINE_STAMP_DIR}
        COMMAND ${CMAKE_COMMAND} -E touch ${ASSET_PIPELINE_STAMP}
        COMMENT "Run asset pipeline.")
add_custom_target(run_asset_pipeline DEPENDS asset_pipeline.stamp)
add_dependencies(run_asset_pipeline copy_assets asset_pipeline)

# add preprocessor definitions
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
ADD_DEFINITIONS(-DSRC_PATH="${CMAKE_CURRENT_SOURCE_DIR}/${C_SRC_DIR}")
ADD_DEFINITIONS(-DVULKAN_HEADER_PATH="${VULKAN_INCLUDE_PATH}/vulkan/vulkan_core.h")

# Compiler and linker flags for ASAN.
set(ASAN_FLAGS "$<$<CONFIG:DEBUG>:-static-libstdc++>" "$<$<CONFIG:DEBUG>:-static-libasan>" "$<$<CONFIG:DEBUG>:-fsanitize=address" "-fsanitize=undefined" "-fno-omit-frame-pointer>")
# Compiler and linker flags for hardening.
set(HARDENING_FLAGS "-fstack-protector-all")

# define targets
message(STATUS "PYTHON_CODEGEN_SCRIPTS: ${PYTHON_CODEGEN_SCRIPTS}")
message(STATUS "PYTHON_ASSET_SCRIPTS: ${PYTHON_ASSET_SCRIPTS}")
message(STATUS "ASSETS_SHADER_FILES: ${ASSETS_SHADER_FILES}")
message(STATUS "ASSETS_GLTF_FILES: ${ASSETS_GLTF_FILES}")
message(STATUS "ASSETS_CUBEMAP_FILES: ${ASSETS_CUBEMAP_FILES}")
message(STATUS "C_HEADERS: ${C_HEADERS}")
message(STATUS "C_CODEGEN_HEADERS: ${C_CODEGEN_HEADERS}")
message(STATUS "C_SOURCES: ${C_SOURCES}")
message(STATUS "C_BIN_SOURCES: ${C_BIN_SOURCES}")
message(STATUS "C_PRECOMPILED_HEADERS: ${C_PRECOMPILED_HEADERS}")
message(STATUS "C_CODEGEN_SOURCES: ${C_CODEGEN_SOURCES}")

function(my_add_compiler_options target)
    if ((CMAKE_C_COMPILER_ID STREQUAL "GNU") OR (CMAKE_C_COMPILER_ID STREQUAL "Clang"))
        target_compile_options(${target} PUBLIC ${ASAN_FLAGS})
        target_link_options(${target} PUBLIC ${ASAN_FLAGS})
    endif ()
    if ((CMAKE_C_COMPILER_ID STREQUAL "GNU") OR (CMAKE_C_COMPILER_ID STREQUAL "Clang"))
        target_compile_options(${target} PUBLIC
                -Wall -Wextra -pedantic
                -Werror
                ${HARDENING_FLAGS}
                -Wno-unused-parameter -Wno-unused-variable)
        target_link_options(${target} PUBLIC
                ${HARDENING_FLAGS})
    elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/W4 /WX)
    else ()
        message(FATAL_ERROR "unsupported compiler")
    endif ()
endfunction()

# library
set(ENGINE_NAME engine)

add_library(${ENGINE_NAME})
my_add_compiler_options(${ENGINE_NAME})

target_sources(${ENGINE_NAME} PUBLIC ${EXTERNAL_ADDITIONAL_SOURCES})
target_link_directories(${ENGINE_NAME} PUBLIC ${EXTERNAL_LIBRARY_PATHS})
target_link_libraries(${ENGINE_NAME} PUBLIC ${EXTERNAL_LIBRARIES})

target_precompile_headers(${ENGINE_NAME} PUBLIC ${C_PRECOMPILED_HEADERS})
target_include_directories(${ENGINE_NAME} PUBLIC ${C_SRC_DIR} PUBLIC ${EXTERNAL_INCLUDE_PATHS})
target_sources(${ENGINE_NAME} PUBLIC ${C_SOURCES} ${C_HEADERS} ${C_CODEGEN_HEADERS} ${C_CODEGEN_SOURCES})

# executables
foreach (C_BIN_SOURCE_PATH ${C_BIN_SOURCES})
    get_filename_component(EXECUTABLE_NAME ${C_BIN_SOURCE_PATH} NAME_WE)
    add_executable(${EXECUTABLE_NAME} ${C_BIN_SOURCE_PATH})
    add_dependencies(${EXECUTABLE_NAME} copy_assets)
    if (NOT ${EXECUTABLE_NAME} STREQUAL "asset_pipeline")
        add_dependencies(${EXECUTABLE_NAME} run_asset_pipeline)
    endif ()
    my_add_compiler_options(${EXECUTABLE_NAME})
    target_link_libraries(${EXECUTABLE_NAME} engine)
endforeach ()


# add every file to VS project file structure
macro(GroupSources curdir)
    file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)
    foreach (child ${children})
        if (IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
            GroupSources(${curdir}/${child})
        else ()
            string(REPLACE "/" "\\" groupname ${curdir})
            source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
        endif ()
    endforeach ()
endmacro()
GroupSources(Source)
