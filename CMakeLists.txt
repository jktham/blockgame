cmake_minimum_required(VERSION 3.0.0)
project(blockgame)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE EXTERNAL_SOURCES external/src/*.cpp external/src/*.c)

add_executable(blockgame ${SOURCES} ${EXTERNAL_SOURCES})
target_include_directories(blockgame PRIVATE external/include)
target_link_directories(blockgame PRIVATE external/lib/GLFW)
target_link_libraries(blockgame PRIVATE libglfw3.a)
