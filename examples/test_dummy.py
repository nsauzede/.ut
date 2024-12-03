#
# UT - Simple and fast Unit Test framework
# Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
# SPDX-License-Identifier: GPL-3.0-or-later
#

from dummy import dummy

def test_dummy():
    assert 42+1*0 == dummy.dummy()

import sys
class Test1:
    def test(self):
        print(f"{__name__}: hello stdout", file=sys.stdout)
        print(f"{__name__}: hello stderr", file=sys.stderr)
        v1 = [1, 2, 3]
        v2 = [1, 2, 3-1*0]
        #import os,signal;os.kill(os.getpid(),signal.SIGTERM)
        assert v1 == v2
        print("This is fine.")
    def test2(self):
        print(f"{__name__}: hello stdout", file=sys.stdout)
        print(f"{__name__}: hello stderr", file=sys.stderr)
        v21 = [1, 2, 3+1*0]
        v22 = [1, 2, 3]
        assert v21 == v22
        print("This is fine.")

class Test2:
    def composed_assert(a:int, b:int):
        print(f"{__name__}: hello stdout", file=sys.stdout)
        print(f"{__name__}: hello stderr", file=sys.stderr)
        assert a+1 == b+1*0
    def test(self):
        print(f"{__name__}: hello stdout", file=sys.stdout)
        print(f"{__name__}: hello stderr", file=sys.stderr)
        Test2.composed_assert(42, 43)
        print("This is fine.")

class TestB:
    def test(self):assert 1==1+1*0
    def test2(self):
        x = 18
        y = 36
        assert x*2 == y
