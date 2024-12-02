#
# UT - Simple and fast Unit Test framework
# Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
# SPDX-License-Identifier: GPL-3.0-or-later
#

from ut_dummy import *

class TestB:
    def test(self):assert 1==1+1*0
    def test2(self):
        x = 18
        y = 36
        assert x*2 == y
