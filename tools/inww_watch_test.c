/*
* UT - Simple and fast Unit Test framework
* Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "inww_watch.c"

/******************************************************************************/
#include "ut/ut.h"

TESTMETHOD(test_watch_fails_on_nonexisting) {
    int ret = watch("NONEXISTING");
    printf("watch did return %d\n", ret);
    ASSERT_EQ(1, ret);
}
#if 0
TESTMETHOD(test_watch_manual) {
    printf("Waiting some local file change..\n");
    int ret = watch(".");
    printf("watch did return %d\n", ret);
    ASSERT_EQ(0, ret);
}
#endif
