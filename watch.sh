#!/bin/bash

#
# UT - Simple and fast Unit Test framework
# Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
# SPDX-License-Identifier: GPL-3.0-or-later
#

#UTARGS=$*
RE='\(MODIFY\|CREATE\) .*\.\(c\|cpp\|h\)$'
REPY='\(MODIFY\|CREATE\) .*\.\(py\)$'

export BASEDIR="$(dirname "$(realpath "${BASH_SOURCE[0]}")")"
export MAKEFILE="${BASEDIR}/Makefile"
if [ "${V}" = "1" ]; then
    SILENCEMAKE=--no-print-directory
else
    SILENCEMAKE=-s
fi
export make="make -f ${MAKEFILE} ${SILENCEMAKE}"
export pytest=pytest
[ ! -n "${UT_PROJ}" ] && UT_PROJ=${PWD}

PYUTO="--tb=short"
if [ "x${UT_VERBOSE}" = "x1" ]; then
PYUTO+=" -v"
else
PYUTO+=" --no-header"
fi

[ `uname -o` = "Msys" ] && WIN32=1 || WIN32=0

red="\x1b[0;31m"
bred="\x1b[1;31m"
green="\x1b[0;32m"
bgreen="\x1b[1;32m"
yellow="\x1b[0;33m"
byellow="\x1b[1;33m"
bblue="\x1b[1;34m"
bwhite="\x1b[1;37m"
nrm="\x1b[0m"
step=0
function separator {
    printf "\n\n\n\n\n\n\n\n"
#    printf "\n\n\n\n\n\n\n\n"
#    printf "\n\n\n\n\n\n\n\n"
}
function tdd_status {
    if [ "$1" = "0" ]; then
        printf "\n${bgreen}[=> SUCCESS <=]${nrm}"
    else
        printf "\n${bred}[=> FAILURE <=]${nrm}"
    fi
    printf " ${bwhite}<----- Proceed with TDD cycle..${nrm} "\
"(${bred}Red${nrm}, ${bgreen}Green${nrm}, ${bblue}Refactor${nrm}) #${step}\n"
    ((step=$step+1))
}
function change_detected {
    printf "${bwhite}---------------------> "\
"Re{build|test}ing..${nrm} [$1]\n\n"
}
function trybuild {
#    echo "CHECKING make dry-run"
    UT_PROJ="${UT_PROJ}" UT_INCLUDES="${UT_INCLUDES}" ${make} --dry-run --quiet fast | grep -q . || return 0
#    echo "done"
#    time {

#    separator
    change_detected "$1"
    UT_PROJ="${UT_PROJ}" UT_INCLUDES="${UT_INCLUDES}" UT_FAST="${UT_FAST}" ${make} ${UTARGS}
#    }
    ret=$?
    tdd_status $ret
    return $ret
}

function trypy {
    find . -name "test_*.py" -o -name "*_test.py" | grep -q . || return 0
#    separator
    change_detected "$1"
    ${pytest} ${PYUTO}
    ret=$?
    tdd_status $ret
    return $ret
}

if [ $WIN32 = 1 ]; then
# Windows
${make} mrproper inww.exe || (echo "Can't build required inww.exe"; exit 1)
trybuild "From scratch"
${pytest} || exit 1
################################################################################
while true; do ./inww.exe | (while read changed ; do
    echo "$changed" | grep "$RE" 2>&1 > /dev/null || continue
    trybuild "$changed"
    done)
done

else

# Linux
if [ -z "$(which inotifywait)" ]; then
    echo "Requirement: install `inotify-tools`"
    exit 1
fi
${make} mrproper
trypy "All Python" -q
trybuild "All C/C++"
################################################################################
inotifywait -q --recursive --monitor --format "%e %w%f" \
--exclude '#' \
--event modify,move,create,delete ${UT_PROJ} ${BASEDIR} \
| while read changed; do
#    echo "changed=${changed}"
    echo "$changed" | grep "$RE" 2>&1 > /dev/null && trybuild "$changed" && continue
    echo "$changed" | grep "$REPY" 2>&1 > /dev/null && trypy "$changed"
done

fi
