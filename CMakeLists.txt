cmake_minimum_required(VERSION 3.16)
project(Compiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Standard per-configuration flags. The DEBUG_BUILD define gates the extra
# diagnostics compiled into the sources (instruction tracing in the VM, etc.).
if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG   "/Od /Zi /DDEBUG_BUILD")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3 -DDEBUG_BUILD -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

    # Opt-in sanitizers for Debug builds: cmake -DENABLE_SANITIZERS=ON
    option(ENABLE_SANITIZERS "Enable Address/UB sanitizers for Debug builds" OFF)
    if(ENABLE_SANITIZERS)
        string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fsanitize=address,undefined")
        add_link_options($<$<CONFIG:Debug>:-fsanitize=address,undefined>)
    endif()
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
