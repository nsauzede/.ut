#!/usr/bin/env python3

#
# UT - Simple and fast Unit Test framework
# Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
# SPDX-License-Identifier: GPL-3.0-or-later
#

from pathlib import Path
import os
import sys

def mkdep(cachedir:str, inp:str)->str:
    outp = ""
    inp = inp.replace("C:/","/c/")
    inp = inp.replace("\r\n","\n")
    inp = inp.replace("\\\n","")
    inp = inp.replace("\n","")
    while "  " in inp:
        inp = inp.replace("  "," ")
    l=inp.split(': ')[1].split(' ')
    tgt = l[0]
    tgts = f"{cachedir}/{tgt}.fast.exe {cachedir}/{tgt}.slow.exe"
    deps = []
    for p_ in l:
        p = Path(p_)
        if p.is_absolute():
            canonical_path = p
        else:
            canonical_path = Path(os.path.normpath(p))
        deps += [canonical_path]
    outp += f"{tgts}: CFLAGS+="
    sp = ""
    for dep in deps[1:]:
        if not dep.is_absolute():
            parent = dep.parent
            outp += f"{sp}-I{parent}"
            sp = " "
    outp += "\n"
    outp += f"{tgts}: "
    sp = ""
    for dep in deps:
            outp += f"{sp}{dep}"
            sp = " "
    outp += "\n"
    return outp

def main():
    cachedir = ".ut/cache"
    inp = sys.stdin.read()
    outp = mkdep(cachedir, inp)
    print(f"{outp}", end="")

if __name__ == '__main__':
    main()
