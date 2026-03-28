// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#include "vedx64/assembler.hpp"
#include "vedx64/core.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

using namespace vedx64;

static std::string read_stdin() {
    std::string result;
    char buf[4096];
    while (size_t n = fread(buf, 1, sizeof(buf), stdin)) result.append(buf, n);
    return result;
}

static std::string read_file(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) { fprintf(stderr, "Cannot open %s\n", path); exit(1); }
    std::string result;
    char buf[4096];
    while (size_t n = fread(buf, 1, sizeof(buf), f)) result.append(buf, n);
    fclose(f);
    return result;
}

int main(int argc, char** argv) {
    const char* input_file = nullptr;
    const char* output_file = nullptr;
    bool hex_mode = false;
    bool disasm_mode = false;
    bool single_mode = false;
    const char* single_text = nullptr;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0 && i+1 < argc) output_file = argv[++i];
        else if (strcmp(argv[i], "--hex") == 0) hex_mode = true;
        else if (strcmp(argv[i], "--disasm") == 0 || strcmp(argv[i], "-d") == 0) disasm_mode = true;
        else if (strcmp(argv[i], "-e") == 0 && i+1 < argc) { single_mode = true; single_text = argv[++i]; }
        else if (argv[i][0] != '-') input_file = argv[i];
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("Usage: vedx64_asm [options] [input.asm]\n");
            printf("  -e TEXT     Assemble single instruction\n");
            printf("  -o FILE     Write binary output to file\n");
            printf("  --hex       Print hex dump\n");
            printf("  -d          Disassemble output\n");
            printf("  -h          Show this help\n");
            printf("\nReads from stdin if no input file given.\n");
            printf("Writes binary to stdout if no -o or --hex.\n");
            return 0;
        }
    }

    std::string text;
    if (single_mode) text = single_text;
    else if (input_file) text = read_file(input_file);
    else text = read_stdin();

    std::string error;
    std::optional<std::vector<uint8_t>> result;
    if (single_mode) { result = assemble(text, error); }
    else { result = assemble_block(text); if (!result) error = "block assembly failed"; }
    if (!result) {
        fprintf(stderr, "Error: %s\n", error.c_str());
        return 1;
    }

    if (hex_mode || disasm_mode) {
        if (hex_mode) {
            for (size_t i = 0; i < result->size(); ++i) {
                printf("%02X", (*result)[i]);
                if ((i + 1) % 16 == 0) printf("\n"); else printf(" ");
            }
            if (result->size() % 16 != 0) printf("\n");
        }
#ifdef VEDX64_STRINGS
        if (disasm_mode) {
            size_t off = 0;
            while (off < result->size()) {
                char dis[256];
                size_t n = disassemble(result->data() + off, result->size() - off, dis, sizeof(dis), off);
                if (!n) { printf("%08zx: ?? (decode error)\n", off); off++; continue; }
                printf("%08zx: ", off);
                for (size_t j = 0; j < n; ++j) printf("%02X ", result->data()[off + j]);
                for (size_t j = n; j < 12; ++j) printf("   ");
                printf("%s\n", dis);
                off += n;
            }
        }
#endif
    }

    if (output_file) {
        FILE* f = fopen(output_file, "wb");
        if (!f) { fprintf(stderr, "Cannot open %s for writing\n", output_file); return 1; }
        fwrite(result->data(), 1, result->size(), f);
        fclose(f);
        fprintf(stderr, "Wrote %zu bytes to %s\n", result->size(), output_file);
    } else if (!hex_mode && !disasm_mode) {
#ifdef _WIN32
        _setmode(_fileno(stdout), 0x8000); // _O_BINARY
#endif
        fwrite(result->data(), 1, result->size(), stdout);
    }

    return 0;
}
