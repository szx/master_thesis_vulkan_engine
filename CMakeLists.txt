cmake_minimum_required(VERSION 3.5)

project(cpptest LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# add headers from Vulkan SDK
set(VULKAN_SDK_PATH "$ENV{VULKAN_SDK}")
set(VULKAN_INCLUDE_PATH "${VULKAN_SDK_PATH}/include")
set(VULKAN_LIBRARY_PATH "${VULKAN_SDK_PATH}/lib")

# add external library glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/glfw)

# add external library greatest
set(GREATEST_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/greatest")

# add external library CLT
set(CTL_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ctl/ctl")

# add external library log.c
set(LOGC_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/log.c/src")
set(LOGC_ADDITIONAL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/log.c/src/log.c")

# add external library cgltf
set(CGLTF_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cgltf")

# add external library GLib
find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)

message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
#if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
# add external library gtk4
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
#endif()

# add external library sqlite
pkg_check_modules(SQLITE3 REQUIRED IMPORTED_TARGET sqlite3)

# prepare info about external libraries
set(EXTERNAL_INCLUDE_PATHS ${VULKAN_INCLUDE_PATH} ${GREATEST_INCLUDE_PATH} ${CTL_INCLUDE_PATH}
        ${GTK4_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIRS} ${LOGC_INCLUDE_PATH} ${CGLTF_INCLUDE_PATH})
set(EXTERNAL_ADDITIONAL_SOURCES ${LOGC_ADDITIONAL_SOURCES})
set(EXTERNAL_LIBRARY_PATHS ${VULKAN_LIBRARY_PATH} ${GTK4_LIBRARY_DIRS} ${SQLITE3_LIBRARY_DIRS})
set(EXTERNAL_LIBRARIES glfw PkgConfig::deps ${GTK4_LIBRARIES} ${SQLITE3_LIBRARIES})


# build packcc during configure
if (WIN32)
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(PACKCC_VERSION "mingw-gcc")
    elseif (CMAKE_C_COMPILER_ID STREQUAL "Clang")
        set(PACKCC_VERSION "mingw-clang")
    elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        set(PACKCC_VERSION "msvc")
    else()
        message(FATAL_ERROR "packcc: unsupported compiler")
    endif()
else()
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(PACKCC_VERSION "gcc")
    elseif (CMAKE_C_COMPILER_ID STREQUAL "Clang")
        set(PACKCC_VERSION "clang")
    else()
        message(FATAL_ERROR "packcc: unsupported compiler")
    endif ()
endif ()
execute_process(COMMAND make WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/packcc/build/${PACKCC_VERSION}")
set(PACKCC_BIN "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/packcc/build/${PACKCC_VERSION}/release/bin/packcc")
message(STATUS "PACKCC_BIN: ${PACKCC_BIN}")


# define source files
file(GLOB_RECURSE C_SOURCES "src/*.c")
file(GLOB_RECURSE C_BIN_SOURCES "src/bin/*.c")
foreach (C_BIN_SOURCE_PATH ${C_BIN_SOURCES})
    list(REMOVE_ITEM C_SOURCES "${C_BIN_SOURCE_PATH}")
endforeach ()
file(GLOB_RECURSE C_CODEGEN_SOURCES "src/codegen/*.c")
foreach (C_CODEGEN_SOURCE_PATH ${C_CODEGEN_SOURCES})
    list(REMOVE_ITEM C_SOURCES "${C_CODEGEN_SOURCE_PATH}")
endforeach ()
file(GLOB_RECURSE C_HEADERS "src/*.h")
file(GLOB TEST_FILES_DIR "test")
file(GLOB_RECURSE TEST_FILES "test/*.*")
file(GLOB ASSETS_FILES_DIR "assets")
file(GLOB_RECURSE ASSETS_FILES "assets/*.*")

# define packcc parser generation
file(GLOB_RECURSE C_GENERATOR_PEGS "src/*.peg")
set(C_GENERATOR_PEGS_OUTPUTS)
foreach (PEG ${C_GENERATOR_PEGS})
    string(REGEX REPLACE "[.]peg$" ".c" OUTPUT_SOURCE ${PEG})
    string(REGEX REPLACE "[.]peg$" ".h" OUTPUT_HEADER ${PEG})
    add_custom_command(
            OUTPUT ${OUTPUT_SOURCE} ${OUTPUT_HEADER}
            DEPENDS ${PEG}
            COMMAND ${PACKCC_BIN} ${PEG}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src
            COMMENT "Generating ${PEG} outputs"
            VERBATIM)
    list(APPEND C_GENERATOR_PEGS_OUTPUTS ${OUTPUT_SOURCE} ${OUTPUT_HEADER})
endforeach ()
message(STATUS "C_GENERATOR_PEGS_OUTPUTS: ${C_GENERATOR_PEGS_OUTPUTS}")
add_custom_target(
        codegen_packcc
        ALL
        DEPENDS ${C_GENERATOR_PEGS}
        SOURCES ${C_GENERATOR_PEGS})
set_source_files_properties(${C_GENERATOR_PEGS_OUTPUTS} PROPERTIES GENERATED TRUE)


# copy resources to directory with executables
add_custom_target(copy_resources ALL DEPENDS ${TEST_FILES} ${ASSETS_FILES})
add_custom_command(
        TARGET copy_resources PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${TEST_FILES_DIR} ${CMAKE_CURRENT_BINARY_DIR}/tests
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_FILES_DIR} ${CMAKE_CURRENT_BINARY_DIR}/assets)

# add compiler options
if ((CMAKE_C_COMPILER_ID STREQUAL "GNU") OR (CMAKE_C_COMPILER_ID STREQUAL "Clang"))
    add_compile_options(-Wall -Wextra -pedantic
            -Werror
            -Wno-unused-parameter -Wno-unused-variable)
elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4 /WX)
else ()
    message(FATAL_ERROR "unsupported compiler")
endif ()

# add preprocessor definitions
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
ADD_DEFINITIONS(-DSRC_PATH="${CMAKE_CURRENT_SOURCE_DIR}/src")
ADD_DEFINITIONS(-DVULKAN_HEADER_PATH="${VULKAN_INCLUDE_PATH}/vulkan/vulkan_core.h")

# define executables
message(STATUS "C_HEADERS: ${C_HEADERS}")
message(STATUS "C_SOURCES: ${C_SOURCES}")
message(STATUS "C_BIN_SOURCES: ${C_BIN_SOURCES}")
message(STATUS "C_CODEGEN_SOURCES: ${C_CODEGEN_SOURCES}")
foreach (C_BIN_SOURCE_PATH ${C_BIN_SOURCES})
    get_filename_component(EXECUTABLE_NAME ${C_BIN_SOURCE_PATH} NAME_WE)
    add_executable(${EXECUTABLE_NAME} ${C_BIN_SOURCE_PATH} ${C_SOURCES} ${C_HEADERS} ${C_GENERATOR_PEGS_OUTPUTS})
    target_include_directories(${EXECUTABLE_NAME} PUBLIC ${EXTERNAL_INCLUDE_PATHS})
    target_sources(${EXECUTABLE_NAME} PUBLIC ${EXTERNAL_ADDITIONAL_SOURCES})
    target_link_directories(${EXECUTABLE_NAME} PUBLIC ${EXTERNAL_LIBRARY_PATHS})
    target_link_libraries(${EXECUTABLE_NAME} LINK_PUBLIC ${EXTERNAL_LIBRARIES})
    add_dependencies(${EXECUTABLE_NAME} codegen_packcc)
    add_dependencies(${EXECUTABLE_NAME} copy_resources)
    if (NOT ${EXECUTABLE_NAME} STREQUAL "codegen")
        add_dependencies(${EXECUTABLE_NAME} codegen)
    endif()
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
        endif()
    endforeach()
endmacro()
GroupSources(Source)
