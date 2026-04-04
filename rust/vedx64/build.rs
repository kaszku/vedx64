// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

use std::path::PathBuf;

fn main() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR"))
        .join("..")
        .join("..");

    let include_dir = root.join("include");
    let lib_dir = root.join("lib");

    let mut cpp_sources: Vec<PathBuf> = Vec::new();
    if let Ok(entries) = std::fs::read_dir(lib_dir.join("core")) {
        for entry in entries {
            let path = entry.unwrap().path();
            if path.extension().map_or(false, |e| e == "cpp") {
                cpp_sources.push(path);
            }
        }
    }
    if let Ok(entries) = std::fs::read_dir(lib_dir.join("assembler")) {
        for entry in entries {
            let path = entry.unwrap().path();
            if path.extension().map_or(false, |e| e == "cpp") {
                cpp_sources.push(path);
            }
        }
    }
    if let Ok(entries) = std::fs::read_dir(lib_dir.join("ir")) {
        for entry in entries {
            let path = entry.unwrap().path();
            if path.extension().map_or(false, |e| e == "cpp") {
                cpp_sources.push(path);
            }
        }
    }
    if let Ok(entries) = std::fs::read_dir(lib_dir.join("emu")) {
        for entry in entries {
            let path = entry.unwrap().path();
            if path.extension().map_or(false, |e| e == "cpp") {
                cpp_sources.push(path);
            }
        }
    }
    if let Ok(entries) = std::fs::read_dir(lib_dir.join("relocation")) {
        for entry in entries {
            let path = entry.unwrap().path();
            if path.extension().map_or(false, |e| e == "cpp") {
                cpp_sources.push(path);
            }
        }
    }
    if let Ok(entries) = std::fs::read_dir(lib_dir.join("branch_follow")) {
        for entry in entries {
            let path = entry.unwrap().path();
            if path.extension().map_or(false, |e| e == "cpp") {
                cpp_sources.push(path);
            }
        }
    }
    if let Ok(entries) = std::fs::read_dir(lib_dir.join("hook")) {
        for entry in entries {
            let path = entry.unwrap().path();
            if path.extension().map_or(false, |e| e == "cpp") {
                if !cfg!(target_os = "windows") && path.file_name().unwrap().to_str().unwrap().contains("syscall") { continue; }
                cpp_sources.push(path);
            }
        }
    }
    if let Ok(entries) = std::fs::read_dir(lib_dir.join("syscall")) {
        for entry in entries {
            let path = entry.unwrap().path();
            if path.extension().map_or(false, |e| e == "cpp") {
                if !cfg!(target_os = "windows") && path.file_name().unwrap().to_str().unwrap().contains("syscall") { continue; }
                cpp_sources.push(path);
            }
        }
    }

    cc::Build::new()
        .cpp(true)
        .std("c++17")
        .define("VEDX64_STRINGS", None)
        .define("VEDX64_CODEGEN", None)
        .define("VEDX64_ASSEMBLER", None)
        .define("VEDX64_EMU", None)
        .define("VEDX64_HOOK", None)
        .define("VEDX64_IR", None)
        .include(&include_dir)
        .files(&cpp_sources)
        .compile("vedx64_cpp");

    let bridge_cpp = PathBuf::from(env!("CARGO_MANIFEST_DIR"))
        .join("src")
        .join("vedx64_bridge.cpp");
    let bridge_hpp_dir = PathBuf::from(env!("CARGO_MANIFEST_DIR")).join("src");

    cxx_build::bridge("src/bridge.rs")
        .file(&bridge_cpp)
        .std("c++17")
        .define("VEDX64_STRINGS", None)
        .define("VEDX64_CODEGEN", None)
        .define("VEDX64_ASSEMBLER", None)
        .define("VEDX64_EMU", None)
        .define("VEDX64_HOOK", None)
        .define("VEDX64_IR", None)
        .include(&include_dir)
        .include(&bridge_hpp_dir)
        .compile("vedx64_bridge");

    println!("cargo:rerun-if-changed=src/bridge.rs");
    println!("cargo:rerun-if-changed=src/vedx64_bridge.cpp");
    println!("cargo:rerun-if-changed=src/vedx64_bridge.hpp");
}
