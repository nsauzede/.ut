/*
* UT - Simple and fast Unit Test framework
* Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
* SPDX-License-Identifier: GPL-3.0-or-later
*/
/* Simple Windows INotifyWait-like equivalent */
#ifndef _WIN32
#error Windows-only program
#else
#include "inww_watch.c"
int main(int argc, char *argv[]) {
    const char *path = ".";
    if (argc > 1) {
        path = argv[1];
    }
    return watch(path);
}
#endif
