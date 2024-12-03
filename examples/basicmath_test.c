/*
* UT - Simple and fast Unit Test framework
* Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "basicmath.c"

/******************************************************************************/
#include "ut.h"

typedef struct spy_add_s {
    int (*orig_add)(basicmath_t *self, int a, int b);
    unsigned int pos;
    int seq[4][2];
} spy_add_t;
static spy_add_t spy_add_s0;
static int
spy_add0(basicmath_t *self, int a, int b) {
    if (spy_add_s0.pos < (sizeof(spy_add_s0.seq) / sizeof(spy_add_s0.seq[0]))) {
        spy_add_s0.seq[spy_add_s0.pos][0] = a;
        spy_add_s0.seq[spy_add_s0.pos][1] = b;
        spy_add_s0.pos++;
    }
    return spy_add_s0.orig_add(self, a, b);
}

TESTCASE(TestBasicMath)
    TESTMETHOD(test_mult_with_spy_add) {
        spy_add_s0 = (spy_add_t){basicmath.add, 0, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}};
        basicmath.add = spy_add0;
        ASSERT(12 == basicmath.mult(&basicmath, 4, 3));
        basicmath.add = spy_add_s0.orig_add;
        int seq0[][2] = {{0, 3}, {3, 3}, {6, 3}, {9, 3}};
        unsigned int seq0_len = sizeof(seq0) / sizeof(seq0[0]);
        ASSERT(spy_add_s0.pos == seq0_len);
        unsigned int min_pos = spy_add_s0.pos < seq0_len ? spy_add_s0.pos : seq0_len;
        for (unsigned int pos = 0; pos < min_pos; pos++) {
            ASSERT(spy_add_s0.seq[pos][0] == seq0[pos][0]);
            ASSERT(spy_add_s0.seq[pos][1] == seq0[pos][1]);
        }
    }
    TESTMETHOD(test_mult) {
        ASSERT(0 == basicmath.mult(&basicmath, 0, 1));
        ASSERT(4 == basicmath.mult(&basicmath, 2, 2));
        ASSERT(1 == basicmath.mult(&basicmath, 1, 1));
        ASSERT(9 == basicmath.mult(&basicmath, 3, 3));
        ASSERT(10000 == basicmath.mult(&basicmath, 100, 100));
        ASSERT(-1 == basicmath.mult(&basicmath, -1, 1));
        ASSERT(12 == basicmath.mult(&basicmath, -3, -4));
        ASSERT(312 == basicmath.mult(&basicmath, 13, 24));
    }
    int ut_fail_(){return 0;}
    TESTMETHOD(test_mult_might_fail) {
        int fail = ut_fail_();
        ASSERT(42-fail == basicmath.mult(&basicmath, 6, 7));
    }
    TESTMETHOD(test_add_might_fail) {
        int fail = ut_fail_();
        ASSERT(0+fail == basicmath.add(&basicmath, 0, 0));
    }
    TESTMETHOD(test_add) {
        ASSERT(0 == basicmath.add(&basicmath, 0, 0));
        ASSERT(4 == basicmath.add(&basicmath, 2, 2));
        ASSERT(-2 == basicmath.add(&basicmath, 1, -3));
    }
