#!/usr/bin/env python3

from pathlib import Path
import sys

def mkdep(cwd:str, cachedir:str, inp:str)->str:
    outp = ""
    #outp += f"\n\n\n\n# {inp=}\n"
    inp = inp.replace("\\\n","")
    inp = inp.replace("\n","")
    while "  " in inp:
        inp = inp.replace("  "," ")
    l=inp.split(': ')[1].split(' ')
    tgt = l[0]
    tgts = f"{cachedir}/./{tgt}.fast.exe {cachedir}/./{tgt}.slow.exe"
    deps = []
    #outp += f"# {l=}\n\n\n\n\n"
    for p in l:
        if Path(p).is_absolute():
            canonical_path = Path(p)
        else:
            canonical_path = Path(p).resolve().relative_to(Path(cwd))
        deps += [canonical_path]
    outp += f"{tgts}: CFLAGS+="
    sp = ""
    for dep in deps[1:]:
        if not dep.is_absolute():
            outp += f"{sp}-I{dep.parent}"
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
    cwd = Path.cwd()
    cachedir = ".ut/cache"
    inp = sys.stdin.read()
    outp = mkdep(cwd, cachedir, inp)
    print(f"{outp}", end="")

if __name__ == '__main__':
    main()
