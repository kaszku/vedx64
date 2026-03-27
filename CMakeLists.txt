# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Kevin Szkudlapski
# Auto-generated — do not edit

cmake_minimum_required(VERSION 3.15)
project(vedx64 VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /bigobj)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Build options
option(VEDX64_STRINGS "Enable string functions (disassemble, mnemonic_name, status_to_string, dump_hook)" OFF)
option(VEDX64_CODEGEN "Enable CodeGen assembler DSL" OFF)
option(VEDX64_SEMANTICS "Enable instruction semantics" OFF)
option(VEDX64_EMU "Enable CPU emulator" OFF)
option(VEDX64_HOOK "Enable hooking support (hook.cpp, syscall.cpp, ntdll linking)" OFF)
option(VEDX64_TESTS "Build test executables" OFF)
option(VEDX64_EXAMPLES "Build example programs" OFF)
option(VEDX64_PYTHON "Build Python bindings (requires Python + nanobind)" OFF)
option(VEDX64_ASSEMBLER "Enable text assembler" OFF)
option(VEDX64_IR "Enable IR lifting/interpreter" OFF)

# Core library
set(VEDX64_CORE_SOURCES
    lib/core/table.cpp
    lib/core/decode.cpp
    lib/core/encode.cpp
    lib/core/disasm.cpp
    lib/core/encoding_id.cpp
    lib/core/instruction.cpp
    lib/relocation/relocation.cpp
    lib/branch_follow/branch_follow.cpp
)

if(VEDX64_SEMANTICS)
    list(APPEND VEDX64_CORE_SOURCES lib/semantics/semantics.cpp)
endif()
if(VEDX64_EMU)
    list(APPEND VEDX64_CORE_SOURCES lib/emu/emu.cpp)
endif()
if(VEDX64_HOOK)
    list(APPEND VEDX64_CORE_SOURCES
        lib/syscall/syscall.cpp
        lib/hook/hook.cpp
    )
endif()

if(VEDX64_ASSEMBLER)
    list(APPEND VEDX64_CORE_SOURCES
        lib/assembler/assembler.cpp
    )
endif()

if(VEDX64_IR)
    list(APPEND VEDX64_CORE_SOURCES
        lib/ir/ir.cpp
    )
endif()

add_library(vedx64_core STATIC ${VEDX64_CORE_SOURCES})
target_include_directories(vedx64_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
set_target_properties(vedx64_core PROPERTIES FOLDER "lib")

source_group("core" REGULAR_EXPRESSION "lib/core/.*")
source_group("semantics" REGULAR_EXPRESSION "lib/semantics/.*")
source_group("emu" REGULAR_EXPRESSION "lib/emu/.*")
source_group("syscall" REGULAR_EXPRESSION "lib/syscall/.*")
source_group("hook" REGULAR_EXPRESSION "lib/hook/.*")
source_group("relocation" REGULAR_EXPRESSION "lib/relocation/.*")
source_group("branch_follow" REGULAR_EXPRESSION "lib/branch_follow/.*")

if(VEDX64_STRINGS)
    target_compile_definitions(vedx64_core PUBLIC VEDX64_STRINGS)
endif()
if(VEDX64_CODEGEN)
    target_compile_definitions(vedx64_core PUBLIC VEDX64_CODEGEN)
endif()
if(VEDX64_SEMANTICS)
    target_compile_definitions(vedx64_core PUBLIC VEDX64_SEMANTICS)
endif()
if(VEDX64_EMU)
    target_compile_definitions(vedx64_core PUBLIC VEDX64_EMU)
endif()
if(VEDX64_ASSEMBLER)
    target_compile_definitions(vedx64_core PUBLIC VEDX64_ASSEMBLER)
endif()
if(VEDX64_IR)
    target_compile_definitions(vedx64_core PUBLIC VEDX64_IR)
endif()
if(VEDX64_HOOK)
    target_compile_definitions(vedx64_core PUBLIC VEDX64_HOOK)
endif()

# Link ntdll on Windows for NT syscall-based hooking
if(WIN32 AND VEDX64_HOOK)
    target_link_libraries(vedx64_core PUBLIC ntdll)
endif()

if(VEDX64_TESTS)
    enable_testing()

    add_executable(core_test tests/core_test.cpp)
    target_link_libraries(core_test PRIVATE vedx64_core)
    set_target_properties(core_test PROPERTIES FOLDER "tests")
    add_test(NAME core_test COMMAND core_test)

    if(VEDX64_CODEGEN)
        add_executable(codegen_test tests/codegen_test.cpp)
        target_link_libraries(codegen_test PRIVATE vedx64_core)
        set_target_properties(codegen_test PROPERTIES FOLDER "tests")
        add_test(NAME codegen_test COMMAND codegen_test)
    endif()

    if(VEDX64_SEMANTICS)
        add_executable(semantics_test tests/semantics_test.cpp)
        target_link_libraries(semantics_test PRIVATE vedx64_core)
        set_target_properties(semantics_test PROPERTIES FOLDER "tests")
        add_test(NAME semantics_test COMMAND semantics_test)
    endif()

    if(VEDX64_EMU)
        add_executable(emu_test tests/emu_test.cpp)
        target_link_libraries(emu_test PRIVATE vedx64_core)
        set_target_properties(emu_test PROPERTIES FOLDER "tests")
        add_test(NAME emu_test COMMAND emu_test)
    endif()

    add_executable(edge_case_test tests/edge_case_test.cpp)
    target_link_libraries(edge_case_test PRIVATE vedx64_core)
    set_target_properties(edge_case_test PROPERTIES FOLDER "tests")
    add_test(NAME edge_case_test COMMAND edge_case_test)

    add_executable(instruction_test tests/instruction_test.cpp)
    target_link_libraries(instruction_test PRIVATE vedx64_core)
    set_target_properties(instruction_test PROPERTIES FOLDER "tests")
    add_test(NAME instruction_test COMMAND instruction_test)

    add_executable(relocation_test tests/relocation_test.cpp)
    target_link_libraries(relocation_test PRIVATE vedx64_core)
    set_target_properties(relocation_test PROPERTIES FOLDER "tests")
    add_test(NAME relocation_test COMMAND relocation_test)

    add_executable(branch_follow_test tests/branch_follow_test.cpp)
    target_link_libraries(branch_follow_test PRIVATE vedx64_core)
    set_target_properties(branch_follow_test PROPERTIES FOLDER "tests")
    add_test(NAME branch_follow_test COMMAND branch_follow_test)

    add_executable(avx512_test tests/avx512_test.cpp)
    target_link_libraries(avx512_test PRIVATE vedx64_core)
    set_target_properties(avx512_test PROPERTIES FOLDER "tests")
    add_test(NAME avx512_test COMMAND avx512_test)

    if(VEDX64_IR)
        add_executable(ir_test tests/ir_test.cpp)
        target_link_libraries(ir_test PRIVATE vedx64_core)
        set_target_properties(ir_test PROPERTIES FOLDER "tests")
        add_test(NAME ir_test COMMAND ir_test)
    endif()

    if(VEDX64_ASSEMBLER)
        add_executable(assembler_test tests/assembler_test.cpp)
        target_link_libraries(assembler_test PRIVATE vedx64_core)
        set_target_properties(assembler_test PROPERTIES FOLDER "tests")
        add_test(NAME assembler_test COMMAND assembler_test)
    endif()

    if(VEDX64_HOOK)
        add_executable(hook_test tests/hook_test.cpp)
        target_link_libraries(hook_test PRIVATE vedx64_core)
        set_target_properties(hook_test PROPERTIES FOLDER "tests")
        add_test(NAME hook_test COMMAND hook_test)
    endif()
endif()

if(VEDX64_EXAMPLES)
    add_executable(core_example examples/core_example.cpp)
    target_link_libraries(core_example PRIVATE vedx64_core)
    set_target_properties(core_example PROPERTIES FOLDER "examples")

    if(VEDX64_CODEGEN)
        add_executable(codegen_example examples/codegen_example.cpp)
        target_link_libraries(codegen_example PRIVATE vedx64_core)
        set_target_properties(codegen_example PROPERTIES FOLDER "examples")
    endif()

    if(VEDX64_SEMANTICS)
        add_executable(semantics_example examples/semantics_example.cpp)
        target_link_libraries(semantics_example PRIVATE vedx64_core)
        set_target_properties(semantics_example PROPERTIES FOLDER "examples")
    endif()

    if(VEDX64_EMU)
        add_executable(emu_example examples/emu_example.cpp)
        target_link_libraries(emu_example PRIVATE vedx64_core)
        set_target_properties(emu_example PROPERTIES FOLDER "examples")
    endif()

    add_executable(instruction_example examples/instruction_example.cpp)
    target_link_libraries(instruction_example PRIVATE vedx64_core)
    set_target_properties(instruction_example PROPERTIES FOLDER "examples")

    add_executable(relocation_example examples/relocation_example.cpp)
    target_link_libraries(relocation_example PRIVATE vedx64_core)
    set_target_properties(relocation_example PROPERTIES FOLDER "examples")

    add_executable(branch_follow_example examples/branch_follow_example.cpp)
    target_link_libraries(branch_follow_example PRIVATE vedx64_core)
    set_target_properties(branch_follow_example PROPERTIES FOLDER "examples")

    if(VEDX64_ASSEMBLER)
        add_executable(assembler_example examples/assembler_example.cpp)
        target_link_libraries(assembler_example PRIVATE vedx64_core)
        set_target_properties(assembler_example PROPERTIES FOLDER "examples")
    endif()

    if(VEDX64_IR)
        add_executable(ir_example examples/ir_example.cpp)
        target_link_libraries(ir_example PRIVATE vedx64_core)
        set_target_properties(ir_example PROPERTIES FOLDER "examples")
    endif()

    if(VEDX64_HOOK)
        add_executable(hook_example examples/hook_example.cpp)
        target_link_libraries(hook_example PRIVATE vedx64_core)
        set_target_properties(hook_example PROPERTIES FOLDER "examples")
    endif()
endif()

if(VEDX64_PYTHON)
    find_package(Python COMPONENTS Interpreter Development)
    if(Python_FOUND)
        execute_process(
            COMMAND ${Python_EXECUTABLE} -m nanobind --cmake_dir
            OUTPUT_VARIABLE NB_DIR
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(NB_DIR)
            list(APPEND CMAKE_PREFIX_PATH ${NB_DIR})
            find_package(nanobind CONFIG)
            if(nanobind_FOUND)
                nanobind_add_module(vedx64_py python/pybind.cpp)
                target_link_libraries(vedx64_py PRIVATE vedx64_core)
                set_target_properties(vedx64_py PROPERTIES FOLDER "python")
            endif()
        endif()
    endif()
endif()

# Install
install(TARGETS vedx64_core
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)
