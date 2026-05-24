cmake_minimum_required(VERSION 3.16)
project(Compiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(COMPILER_SOURCES
    src/main.cpp
    src/opcodes.cpp
    src/IRGen.cpp
    src/LogicalCode.cpp
    src/CodeGenIR.cpp
    src/optimizer.cpp
    src/assembler.cpp
    src/ExecIO.cpp
    src/loader.cpp
    src/debugger.cpp
    src/vm.cpp
)

add_executable(compiler ${COMPILER_SOURCES})
target_include_directories(compiler PRIVATE include)
