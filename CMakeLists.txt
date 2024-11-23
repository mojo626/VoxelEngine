cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 11)

project(VoxelEngine)

add_subdirectory(lib/bgfx)

add_executable(game src/main.cpp)
target_link_libraries(game PUBLIC SDL3::SDL3)