#!/bin/bash

#
# UT - Simple and fast Unit Test framework
# Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
# SPDX-License-Identifier: GPL-3.0-or-later
#

echo "Fetching TCC.."
\rm -Rf tinycc
git clone https://repo.or.cz/tinycc.git tinycc || exit 1
cd tinycc
echo "Configuring TCC.."
./configure --prefix=`pwd`/the_install
echo "Building/installing TCC.."
make all install
ls -la the_install
echo "Done."
