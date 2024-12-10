#!/usr/bin/env python3

#
# UT - Simple and fast Unit Test framework
# Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
# SPDX-License-Identifier: GPL-3.0-or-later
#

import sys
import os
import signal
import pytest

def get_local_imports(script_path, pyt_args)->(int, list):
    ret = -1
    local_imports = []
    initial_modules = set(sys.modules.keys())
    dirname, filename = os.path.split(script_path)
    args = pyt_args + [filename]
    original_cwd = os.getcwd()
    try:
        os.chdir(dirname)
        ret = pytest.main(args)
    finally:
        os.chdir(original_cwd)
    new_modules = set(sys.modules.keys()) - initial_modules
    current_dir = os.path.abspath(os.path.dirname(script_path))
    for module_name in new_modules:
        module = sys.modules[module_name]
        if hasattr(module, '__file__') and module.__file__:
            module_path = os.path.abspath(module.__file__)
            if module_path.startswith(current_dir):
                local_imports.append(module_path)
    return int(ret), local_imports

if __name__ == "__main__":
    if len(sys.argv) < 5 or sys.argv[1] != "-t" or sys.argv[3] != "-o":
        print(f"Usage: {sys.argv[0]} -t <python_test_file> -o <output_deps_file> [pytest arguments..]")
        sys.exit(1)
    script_path = sys.argv[2]
    out_deps = sys.argv[4]
    if len(sys.argv) > 5:
        pyt_args = sys.argv[5:]
    else:
        pyt_args = []
    ret, local_files = get_local_imports(script_path, pyt_args)
    passed = out_deps.replace(".d", ".passed")
    with open(out_deps, "wt") as f:
        f.write(f"{passed}:")
        for file in local_files:
            f.write(f" {file}")
        f.write("\n")
    # Avoid spurious messages during make -dry-run like:
    #   Exception ignored in: <_io.TextIOWrapper name='<stdout>' mode='w' encoding='utf-8'>
    #   BrokenPipeError: [Errno 32] Broken pipe
    signal.signal(signal.SIGPIPE, signal.SIG_DFL)
    sys.exit(ret)

