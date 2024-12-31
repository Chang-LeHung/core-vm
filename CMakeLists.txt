cmake_minimum_required(VERSION 3.10)
project(core-vm)
enable_language(ASM)
set(CMAKE_CXX_STANDARD 20)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "amd64")
    message(STATUS "This is an x86_64 (amd64) platform")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
    set(
        ASM_FILES
        src/cpu/arm64/trampoline.s
        src/cpu/arm64/done.s
        src/cpu/arm64/iadd.s
        src/cpu/arm64/imul.s
        src/cpu/arm64/idiv.s
        src/cpu/arm64/isub.s
        src/cpu/arm64/loadi.s
        src/cpu/arm64/storei.s
        src/cpu/arm64/printi.s
        src/cpu/arm64/iconst.s
    )
    message(STATUS "This is an ARM64 (aarch64) platform")
else()
    message(FATAL_ERROR "Unsupported platform architecture")
endif()


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SANITIZER_FLAGS "-fsanitize=address,undefined,leak")
    set(CMAKE_CXX_FLAGS "-Wno-unused-result ${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS}")
    set(CMAKE_C_FLAGS " -Wno-unused-result ${CMAKE_C_FLAGS} ${SANITIZER_FLAGS}")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
endif()
set_source_files_properties(${ASM_FILES} PROPERTIES LANGUAGE ASM)

set(SOURCE_FILES
    src/parser.cpp
    src/lexer.cpp
    src/loader.cpp
    src/util.cpp
    src/ir.cpp
    src/assembler.cpp
    src/types.cpp
    src/bytecodeinterpreter.cpp
    src/compiler.cpp
    src/templategenerator.cpp
    src/asminterpreter.cpp
    ${ASM_FILES}
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

include_directories(src/include)
include_directories(src/cpu)

add_executable(core-vm main.cpp ${SOURCE_FILES})

add_executable(test_lexer test/test_lexer.cpp ${SOURCE_FILES})
add_executable(test_parser test/test_parser.cpp ${SOURCE_FILES})
add_executable(test_parser_cast test/test_parser_cast.cpp ${SOURCE_FILES})
add_executable(test_verify test/test_verify.cpp ${SOURCE_FILES})
add_executable(test_asm test/test_asm.cpp ${SOURCE_FILES})
add_executable(test_compiler test/test_compiler.cpp ${SOURCE_FILES})
add_executable(test_interpreter test/test_interpreter.cpp ${SOURCE_FILES})
add_executable(test_arm64 test/test_arm64.cpp ${SOURCE_FILES})


enable_testing()
add_test(NAME test_lexer COMMAND test_lexer ../test/test_lexer.cvm)
add_test(NAME test_parser COMMAND test_parser ../test/test_parser.cvm)
add_test(NAME test_cast COMMAND test_parser_cast ../test/test_cast.cvm)

add_test(NAME test_verify_v1 COMMAND test_verify ../test/test_lexer.cvm)
add_test(NAME test_verify_v2 COMMAND test_verify ../test/test_cast.cvm)
add_test(NAME test_verify_v3 COMMAND test_verify ../test/test_parser.cvm)

add_test(NAME test_asm_v1 COMMAND test_asm ../test/test_lexer.cvm)
add_test(NAME test_asm_v2 COMMAND test_asm ../test/test_cast.cvm)
add_test(NAME test_asm_v3 COMMAND test_asm ../test/test_parser.cvm)


add_test(NAME test_compiler_v1 COMMAND test_compiler ../test/test_lexer.cvm)
add_test(NAME test_compiler_v2 COMMAND test_compiler ../test/test_cast.cvm)
add_test(NAME test_compiler_v3 COMMAND test_compiler ../test/test_parser.cvm)

add_test(NAME test_interpreter_v1 COMMAND test_interpreter ../test/test_lexer.cvm)
add_test(NAME test_interpreter_v2 COMMAND test_interpreter ../test/test_cast.cvm)
add_test(NAME test_interpreter_v3 COMMAND test_interpreter ../test/test_parser.cvm)

add_test(NAME test_arm64 COMMAND test_arm64)
