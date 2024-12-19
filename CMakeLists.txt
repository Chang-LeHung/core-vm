cmake_minimum_required(VERSION 3.10)
project(core-vm)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SANITIZER_FLAGS "-fsanitize=address,undefined,leak")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SANITIZER_FLAGS}")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
endif()

set(SOURCE_FILES
    src/parser.cpp
    src/lexer.cpp
    src/loader.cpp
    src/util.cpp
    src/ir.cpp
    src/assembler.cpp
    src/types.cpp
    src/bytecodeinterpreter.cpp
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

include_directories(src/include)

add_executable(core-vm main.cpp ${SOURCE_FILES})

add_executable(test_lexer test/test_lexer.cpp ${SOURCE_FILES})
add_executable(test_parser test/test_parser.cpp ${SOURCE_FILES})

enable_testing()
add_test(NAME test_lexer COMMAND test_lexer ../test/test_lexer.cvm)
add_test(NAME test_parser_v1 COMMAND test_parser ../test/test_lexer.cvm)
add_test(NAME test_parser_v2 COMMAND test_parser ../test/test_parser.cvm)
