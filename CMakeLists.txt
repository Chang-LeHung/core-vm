cmake_minimum_required(VERSION 3.10)
project(core-vm)

set(CMAKE_CXX_STANDARD 17)

include_directories(src/include())
add_executable(core-vm main.cpp src/parse.cpp)
