cmake_minimum_required(VERSION 3.5)

project(cpptest LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add headers from Vulkan SDK
set(VULKAN_INCLUDE_PATH "$ENV{VULKAN_SDK}/include")
set(VULKAN_LIBRARY_PATH "$ENV{VULKAN_SDK}/lib/vulkan-1.lib")

# add external library glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory( thirdparty/glfw )

set(EXTERNAL_INCLUDE_PATHS ${VULKAN_INCLUDE_PATH})
set(EXTERNAL_LIBRARY_PATHS ${VULKAN_LIBRARY_PATH})
set(EXTERNAL_LIBRARIES glfw)


# build packcc during configure
if (WIN32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(PACKCC_VERSION "mingw-gcc")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(PACKCC_VERSION "mingw-clang")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(PACKCC_VERSION "msvc")
    else()
        message(FATAL_ERROR "packcc: unsupported compiler")
    endif()
else()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(PACKCC_VERSION "gcc")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(PACKCC_VERSION "clang")
    else()
        message(FATAL_ERROR "packcc: unsupported compiler")
    endif()
endif()
execute_process(COMMAND make WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/packcc/build/${PACKCC_VERSION})
set(PACKCC_BIN ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/packcc/build/${PACKCC_VERSION}/release/bin/packcc)
message(STATUS "PACKCC_BIN: ${PACKCC_BIN}")


# define source files
file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")
file(GLOB_RECURSE CPP_HEADERS "src/*.h")
get_filename_component(CPP_MAIN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ABSOLUTE)
list(REMOVE_ITEM CPP_SOURCES "${CPP_MAIN_PATH}")


# define packcc parser generation
file(GLOB_RECURSE CPP_GENERATOR_PEGS "src/*.peg")
set(CPP_GENERATOR_OUTPUTS)
foreach(PEG ${CPP_GENERATOR_PEGS})
    string(REGEX REPLACE "[.]peg$" ".c" OUTPUT_SOURCE ${PEG})
    string(REGEX REPLACE "[.]peg$" ".h" OUTPUT_HEADER ${PEG})
    add_custom_command(
        OUTPUT ${OUTPUT_SOURCE} ${OUTPUT_HEADER}
        DEPENDS ${PEG}
        COMMAND ${PACKCC_BIN} ${PEG}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src
        COMMENT "Generating ${PEG} outputs"
        VERBATIM)
    list(APPEND CPP_GENERATOR_OUTPUTS ${OUTPUT_SOURCE} ${OUTPUT_HEADER})
endforeach()
message(STATUS "CPP_GENERATOR_OUTPUTS: ${CPP_GENERATOR_OUTPUTS}")
add_custom_target(
    codegen_packcc
    ALL
    DEPENDS ${CPP_GENERATOR_PEGS}
    SOURCES ${CPP_GENERATOR_PEGS})
set_source_files_properties(${CPP_GENERATOR_OUTPUTS} PROPERTIES GENERATED TRUE)


# define executable
add_executable(cpptest ${CPP_MAIN_PATH} ${CPP_SOURCES} ${CPP_HEADERS} ${CPP_GENERATOR_OUTPUTS})
target_include_directories(cpptest PUBLIC ${EXTERNAL_INCLUDE_PATHS})
target_link_directories(cpptest PUBLIC ${EXTERNAL_LIBRARY_PATHS})
target_link_libraries(cpptest LINK_PUBLIC ${EXTERNAL_LIBRARIES})
add_dependencies(cpptest codegen_packcc)


# add every file to VS project file structure
macro(GroupSources curdir)
    file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)
    foreach(child ${children})
        if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
            GroupSources(${curdir}/${child})
        else()
            string(REPLACE "/" "\\" groupname ${curdir})
            source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
        endif()
    endforeach()
endmacro()
GroupSources(Source)
