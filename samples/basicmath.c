/*
* UT - Simple and fast Unit Test framework
* Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
* SPDX-License-Identifier: GPL-3.0-or-later
*/

/******************************************************************************/

typedef struct basicmath_s basicmath_t;
typedef struct basicmath_s {
    int (*add)(basicmath_t *self, int a, int b);
    int (*mult)(basicmath_t *self, int a, int b);
} basicmath_t;
extern basicmath_t basicmath;
int
add(basicmath_t *self, int a, int b) {
    return a + b;
}
int
mult(basicmath_t *self, int a, int b) {
    int posA = a >= 0;
    int posB = b >= 0;
    int negP = posA != posB;
    a = posA ? a : -a;
    b = posB ? b : -b;
    int p = 0;
    while (a-- > 0) {
        p = basicmath.add(self, p, b);
    }
    return negP ? -p : p;
}
static int
shiftAndAdd(basicmath_t *self, int a, int b) {
    int p;
    for (p = 0; a > 0; b <<= 1, a >>= 1) {
        if ((a & 1) == 1)
            p = basicmath.add(self, p, b);
    }
    return p;
}
int
mult2(basicmath_t *self, int a, int b) {
    int posA = a >= 0;
    int posB = b >= 0;
    int negP = posA != posB;
    a = posA ? a : -a;
    b = posB ? b : -b;
    int p = shiftAndAdd(self, a, b);
    return negP ? -p : p;
}
int
mult1(basicmath_t *self, int a, int b) {
    return a * b;
}
basicmath_t basicmath = { add, mult };
