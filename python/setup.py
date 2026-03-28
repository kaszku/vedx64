# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Kevin Szkudlapski
# Auto-generated — do not edit

import os
from pathlib import Path
import nanobind
from setuptools import setup, Extension
from nanobind.build import CMakeExtension, CMakeBuild

setup(
    name="vedx64",
    version="0.1.0",
    description="x86-64 instruction toolkit: decoder, encoder, disassembler, assembler, emulator, and more",
    ext_modules=[CMakeExtension("vedx64_py")],
    cmdclass={"build_ext": CMakeBuild},
)
