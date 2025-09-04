#!/bin/bash

#
# UT - Simple and fast Unit Test framework
# Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
# SPDX-License-Identifier: GPL-3.0-or-later
#

RE='\(CREATE\|MODIFY\|MOVE\|MOVED_TO\|DELETE\) .*\.\(c\|cpp\|cc\|h\|py\|v\)$'

SCRIPTSDIR="$(dirname "$(realpath "${BASH_SOURCE[0]}")")"
UTROOT="$(dirname ${SCRIPTSDIR})"
MAKEFILE="${UTROOT}/tools/Makefile"
if [ "${V}" = "1" ]; then
    SILENCEMAKE=--no-print-directory
else
    SILENCEMAKE=-s
fi
export make="make -f ${MAKEFILE} ${SILENCEMAKE}"
export PYTEST=pytest
[ ! -n "${UT_PROJ}" ] && UT_PROJ=${PWD}

PYUTO="--tb=short --no-header"
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
    if [ "x$1" = "x0" ]; then
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
function trytests {
    UT_PROJ="${UT_PROJ}" UT_INCLUDES="${UT_INCLUDES}" ${make} --dry-run all UT_DRY_RUN=1| grep -q . || return 0
    change_detected "$1"
    UT_PROJ="${UT_PROJ}" UT_INCLUDES="${UT_INCLUDES}" UT_FAST="${UT_FAST}" ${make}
    ret=$?
    tdd_status $ret
#    return $ret
    return 1
}

if [ $WIN32 = 1 ]; then
# Windows
RE='\(MODIFY\|CREATE\|ATTRIB\|MOVE\|DELETE\) .*\.\(c\|cpp\|h\|py\)$'
if [ ! -x tools/inww.exe ]; then
    cd ${UTROOT}
    ${make} tools/inww.exe
    if [ "x$?" != "x0" ]; then
        echo "Can't build required inww.exe"
        exit 1
    fi
    cd -
fi
trytests "BOOTSTRAP <all relevant files>" && tdd_status 0
################################################################################
while true; do ${UTROOT}/tools/inww.exe | (while read changed ; do
#    echo "changed=${changed}"
#    changed=`echo ${changed} | sed 's/\\\/\//'`
#    echo "changed=${changed}"
    echo "$changed" | grep "$RE" 2>&1 > /dev/null && trytests "$changed" #|| echo "NOPE.."
    done)
done

else

# Linux
if [ -z "$(which inotifywait)" ]; then
    echo "Requirement: install `inotify-tools`"
    exit 1
fi
trytests "BOOTSTRAP <all relevant files>" && tdd_status 0
################################################################################
evlist="modify,create,attrib,move,delete"
t0="-1"
inotifywait -q --recursive --monitor --timefmt "%S" --format "%T %e %w%f" \
--exclude '#' \
--event ${evlist} ${UT_PROJ} ${UTROOT} \
| while read ev; do
    t=$(echo "$ev" | cut -c1-2)
    if [ "x$t" = "x$t0" ]; then
        #echo -n "Ignore t=$t t0=$t0; "
        continue
    fi
    changed=$(echo "$ev" | cut -c4-)
    echo "$changed" | grep "$RE" 2>&1 > /dev/null && t0=$t && trytests "$changed - $t" </dev/tty >/dev/tty 2>&1
done

fi
