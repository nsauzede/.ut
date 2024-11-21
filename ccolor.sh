#!/bin/bash

#
# UT - Simple and fast Unit Test framework
# Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
# SPDX-License-Identifier: GPL-3.0-or-later
#

bred="\x1b[1;31m"
bwhite="\x1b[1;37m"
bcyan="\x1b[1;36m"
nrm="\x1b[0m"
sed -e "s/\(error:\)/$bred\1$nrm/g" \
    -e "s/\(^.*:.*:\)/$bwhite\1$nrm/g" \
    -e "s/'\(.*\)'/'$bwhite\1$nrm'/g" \
    -e "s/\(note:\)/$bcyan\1$nrm/g"
