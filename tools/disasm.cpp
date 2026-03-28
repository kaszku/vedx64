// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/core.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>

using namespace vedx64;

static int hex_nibble(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static std::vector<uint8_t> parse_hex(const char* s) {
    std::vector<uint8_t> bytes;
    while (*s) {
        while (*s == ' ' || *s == '\t' || *s == ',' || *s == '\n' || *s == '\r') s++;
        if (!*s) break;
        int hi = hex_nibble(*s++);
        if (hi < 0 || !*s) break;
        int lo = hex_nibble(*s++);
        if (lo < 0) break;
        bytes.push_back((uint8_t)((hi << 4) | lo));
    }
    return bytes;
}

static std::vector<uint8_t> read_file(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return {};
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    std::vector<uint8_t> data(size);
    fread(data.data(), 1, size, f);
    fclose(f);
    return data;
}

int main(int argc, char** argv) {
    uint64_t base_addr = 0;
    std::vector<uint8_t> code;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--base") == 0 && i + 1 < argc) {
            base_addr = strtoull(argv[++i], nullptr, 0);
        } else if (strcmp(argv[i], "--file") == 0 && i + 1 < argc) {
            code = read_file(argv[++i]);
            if (code.empty()) {
                fprintf(stderr, "Failed to read file: %s\n", argv[i]);
                return 1;
            }
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("Usage: vedx64-disasm [OPTIONS] [hex bytes...]\n");
            printf("  --base <addr>   Set base address (default 0)\n");
            printf("  --file <path>   Disassemble binary file\n");
            printf("  hex bytes       Hex string to disassemble (e.g. 4889E5)\n");
            return 0;
        } else {
            // Treat as hex input
            auto bytes = parse_hex(argv[i]);
            code.insert(code.end(), bytes.begin(), bytes.end());
        }
    }

    if (code.empty()) {
        // Read hex from stdin
        std::string line;
        char buf[4096];
        while (fgets(buf, sizeof(buf), stdin)) {
            auto bytes = parse_hex(buf);
            code.insert(code.end(), bytes.begin(), bytes.end());
        }
    }

    if (code.empty()) {
        fprintf(stderr, "No input. Use --help for usage.\n");
        return 1;
    }

    size_t offset = 0;
    char disasm_buf[256];
    while (offset < code.size()) {
        uint64_t rip = base_addr + offset;
        size_t remaining = code.size() - offset;
        size_t n = disassemble(code.data() + offset, remaining, disasm_buf, sizeof(disasm_buf), rip);
        if (n == 0) {
            printf("%016llx  %02x                    ???\n",
                   (unsigned long long)rip, code[offset]);
            n = 1;
        } else {
            printf("%016llx  ", (unsigned long long)rip);
            for (size_t j = 0; j < n; ++j) printf("%02x ", code[offset + j]);
            for (size_t j = n; j < 10; ++j) printf("   ");
            printf(" %s\n", disasm_buf);
        }
        offset += n;
    }
    return 0;
}
