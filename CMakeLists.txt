cmake_minimum_required(VERSION 3.5)

project(cpptest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")
file(GLOB_RECURSE CPP_HEADERS "src/*.h")
get_filename_component(CPP_MAIN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ABSOLUTE)
list(REMOVE_ITEM CPP_SOURCES "${CPP_MAIN_PATH}")

# add glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory( thirdparty/glfw )

set(EXTERNAL_INCLUDES "")
set(EXTERNAL_LIBRARIES glfw)

add_executable(cpptest ${CPP_MAIN_PATH} ${CPP_SOURCES} ${CPP_HEADERS})
target_include_directories(cpptest PUBLIC ${EXTERNAL_INCLUDES})
target_link_libraries(cpptest LINK_PUBLIC ${EXTERNAL_LIBRARIES})
