#
# UT - Simple and fast Unit Test framework
# Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
# SPDX-License-Identifier: GPL-3.0-or-later
#

from mkdep import mkdep

def test():
    utdir = "/some/where/git/ut_"
    tstdir = "ut_tests/tests"
    tst = "test_list.c"
    cache = f"{utdir}/.ut/.cache"
    tgts = f"{cache}/{tstdir}/{tst}.fast.exe {cache}/{tstdir}/{tst}.slow.exe"
    inps = [
        f"""test_list.o: {tstdir}/{tst} \\
{tstdir}/../../include/list.h {tstdir}/../../src/list.c \\
{utdir}/ut.h""",
        f"test_list.o: {tstdir}/{tst} {tstdir}/../../include/list.h {tstdir}/../../src/list.c {utdir}/ut.h",
        f"test_list.o:  {tstdir}/{tst}  {tstdir}/../../include/list.h     {tstdir}/../../src/list.c  {utdir}/ut.h",
    ]
    exp_outp = f"""{tgts}: CFLAGS+=-Iinclude -Isrc
{tgts}: {tstdir}/{tst} include/list.h src/list.c {utdir}/ut.h
"""
    def composed_assert(cache, inp, exp_outp):
        out = mkdep(cache, inp)
        assert out == exp_outp
    for inp in inps:
        composed_assert(cache, inp, exp_outp)
