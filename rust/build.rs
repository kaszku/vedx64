// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

fn main() {
    // Link vedx64 static library.
    println!("cargo:rustc-link-search=native={}/../build/Release", env!("CARGO_MANIFEST_DIR"));
    println!("cargo:rustc-link-search=native={}/../build/Debug", env!("CARGO_MANIFEST_DIR"));
    println!("cargo:rustc-link-lib=static=vedx64_core");

    #[cfg(target_os = "windows")]
    {
        println!("cargo:rustc-link-lib=dylib=ntdll");
    }
    #[cfg(not(target_os = "windows"))]
    {
        println!("cargo:rustc-link-lib=dylib=stdc++");
    }

    // Build the cxx bridge.
    cxx_build::bridge("src/lib.rs")
        .file("bridge.cpp")
        .include("../include")
        .include(".")
        .define("VEDX64_STRINGS", None)
        .define("VEDX64_SEMANTICS", None)
        .define("VEDX64_EMU", None)
        .define("VEDX64_IR", None)
        .flag_if_supported("-std=c++17")
        .flag_if_supported("/std:c++17")
        .compile("vedx64_bridge");
}
