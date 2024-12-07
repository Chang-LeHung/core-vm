cmake_minimum_required(VERSION 3.10)
project(core-vm)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
    src/parser.cpp
    src/lexer.cpp
    src/loader.cpp
    src/util.cpp
)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

include_directories(src/include)

add_executable(core-vm main.cpp ${SOURCE_FILES})

add_executable(test_lexer test/test_lexer.cpp ${SOURCE_FILES})
