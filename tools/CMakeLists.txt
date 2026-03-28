# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Kevin Szkudlapski

cmake_minimum_required(VERSION 3.15)
project(vedx64_tools LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Link against the core library from parent output dir
add_library(vedx64_core STATIC
    ../lib/core/table.cpp
    ../lib/core/decode.cpp
    ../lib/core/encode.cpp
    ../lib/core/disasm.cpp
    ../lib/semantics/semantics.cpp
    ../lib/assembler/assembler.cpp
)
target_include_directories(vedx64_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_compile_definitions(vedx64_core PUBLIC VEDX64_STRINGS VEDX64_CODEGEN VEDX64_ASSEMBLER)

add_executable(vedx64_instrgen instrgen.cpp)
target_link_libraries(vedx64_instrgen PRIVATE vedx64_core)

add_executable(vedx64_disasm disasm.cpp)
target_link_libraries(vedx64_disasm PRIVATE vedx64_core)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/asm.cpp)
    add_executable(vedx64_asm asm.cpp)
    target_link_libraries(vedx64_asm PRIVATE vedx64_core)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bench.cpp)
    add_executable(vedx64_bench bench.cpp)
    target_link_libraries(vedx64_bench PRIVATE vedx64_core)
    if(CAPSTONE_INCLUDE AND CAPSTONE_LIB)
        target_include_directories(vedx64_bench PRIVATE ${CAPSTONE_INCLUDE})
        target_link_libraries(vedx64_bench PRIVATE ${CAPSTONE_LIB})
        target_compile_definitions(vedx64_bench PRIVATE HAS_CAPSTONE)
    endif()
endif()
