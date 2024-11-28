from mkdep import mkdep

def test():
    utdir = "/home/nsauzede/perso/git/ut_"
    cwd = utdir
    tstdir = "ut_tests/tests"
    tst = "test_list.c"
    cache = ".ut/cache"
    tgts = f"{cache}/./{tstdir}/{tst}.fast.exe {cache}/./{tstdir}/{tst}.slow.exe"
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
    def composed_assert(cwd, cache, inp, exp_outp):
        out = mkdep(cwd, cache, inp)
        assert out == exp_outp
    for inp in inps:
        composed_assert(cwd, cache, inp, exp_outp)
