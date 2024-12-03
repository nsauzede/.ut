#
# UT - Simple and fast Unit Test framework
# Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
# SPDX-License-Identifier: GPL-3.0-or-later
#

CURRENT_MAKEFILE := $(realpath $(firstword $(MAKEFILE_LIST)))
UTPATH:=$(shell dirname `dirname $(CURRENT_MAKEFILE)`)
UTSCRIPTS:=$(UTPATH)/tools
UT_DIR?=.

MKDIR:=mkdir
VALGRIND:=valgrind
ifneq (0, $(shell $(VALGRIND) --version > /dev/null 2>&1 ; echo $$?))
VALGRIND:=
endif
PYTEST:=pytest
ifneq (0, $(shell $(PYTEST) --version > /dev/null 2>&1 ; echo $$?))
PYTEST:=
endif
TCC:=tcc
#ifneq (0, $(shell $(TCC) --version > /dev/null 2>&1 ; echo $$?))
ifneq (0, $(shell $(TCC) -MM /dev/null > /dev/null 2>&1 ; echo $$?))
TCC:=
endif
#endif
GCC:=gcc
ifneq (0, $(shell $(GCC) --version > /dev/null 2>&1 ; echo $$?))
GCC:=
endif
CLANG:=clang
ifneq (0, $(shell $(CLANG) --version > /dev/null 2>&1 ; echo $$?))
CLANG:=
endif

G++:=g++
ifneq (0, $(shell $(G++) --version > /dev/null 2>&1 ; echo $$?))
G++:=
endif
CLANG++:=clang++
ifneq (0, $(shell $(CLANG++) --version > /dev/null 2>&1 ; echo $$?))
CLANG++:=
endif

ifdef TCC
    CC_FAST:=$(TCC)
else
    ifdef GCC
        CC_FAST:=$(GCC)
    else
        ifdef CLANG
            CC_FAST:=$(CLANG)
        endif
    endif
endif

ifdef CLANG
    CC_SLOW:=$(CLANG)
else
    ifdef GCC
        CC_SLOW:=$(GCC)
    else
        ifdef TCC
            CC_SLOW:=$(TCC)
        endif
    endif
endif

ifdef G++
    CXX_FAST:=$(G++)
else
    ifdef CLANG++
        CXX_FAST:=$(CLANG)
    endif
endif

ifdef CLANG++
    CXX_SLOW:=$(CLANG++)
else
    ifdef G++
        CXX_SLOW:=$(G++)
    endif
endif
ifdef CC_FAST
CC:=$(CC_FAST)
endif
# Add colored output for C compilers with lack thereof (eg: TCC)
CCFILTER:=$(UTSCRIPTS)/ccolor.sh

UT_UT:=.ut
UT_CACHE:=$(UT_UT)/cache

-include $(UT_UT)/ut.mk

CFLAGS:=-Wall -Werror -pipe -I$(UTPATH)/include -DUT_CACHE=\"$(UT_CACHE)\"
CFLAGS+=$(UT_INCLUDES)
CXXFLAGS:=-Wall -Werror -pipe -I$(UTPATH)/include -DUT_CACHE=\"$(UT_CACHE)\"
CXXFLAGS+=$(UT_INCLUDES)

AT_:=@
AT_1:=
AT:=$(AT_$(V))
SILENCEMAKE_:=-s
#SILENCEMAKE_:=--no-print-directory
SILENCEMAKE_1:=
SILENCEMAKE:=$(SILENCEMAKE_$(V))

DEPS:=
ifneq ($(MAKECMDGOALS),mrproper)
C_TESTS:=$(shell find $(UT_DIR) -path '*/.ut' -prune -o \( -type f -name \*.c \) -exec grep -l '^#include "ut.h"' '{}' \;)
CPP_TESTS:=$(shell find $(UT_DIR) -path '*/.ut' -prune -o \( -type f -name \*.cpp \) -exec grep -l '^#include "ut.h"' '{}' \;)
DEPS+=$(patsubst %,$(UT_CACHE)/%.d,$(C_TESTS))
DEPS+=$(patsubst %,$(UT_CACHE)/%.d,$(CPP_TESTS))
endif

EXES:=
PASSED_FAST:=
PASSED_SLOW:=

ifdef CC_FAST
EXES+=$(patsubst %,$(UT_CACHE)/%.fast.exe,$(C_TESTS))
PASSED_FAST+=$(patsubst %,$(UT_CACHE)/%.fast.passed,$(C_TESTS))
endif
ifdef CXX_FAST
EXES+=$(patsubst %,$(UT_CACHE)/%.fast.exe,$(CPP_TESTS))
PASSED_FAST+=$(patsubst %,$(UT_CACHE)/%.fast.passed,$(CPP_TESTS))
endif

ifdef CC_SLOW
EXES+=$(patsubst %,$(UT_CACHE)/%.slow.exe,$(C_TESTS))
PASSED_SLOW+=$(patsubst %,$(UT_CACHE)/%.slow.passed,$(C_TESTS))
endif
ifdef CXX_SLOW
EXES+=$(patsubst %,$(UT_CACHE)/%.slow.exe,$(CPP_TESTS))
PASSED_SLOW+=$(patsubst %,$(UT_CACHE)/%.slow.passed,$(CPP_TESTS))
endif

ifdef VALGRIND
VGO:=-q --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --exit-on-first-error=yes --error-exitcode=128
VG:=$(VALGRIND) $(VGO)
endif

ifdef PYTEST
PYTO:=--tb=short --no-header
ifeq ("x$(UT_VERBOSE)", "x1")
PYTO+=-v
endif
endif

ifeq ("x$(UT_VERBOSE)", "x1")
UTO:=-v
endif

ifdef PYTEST
PYTESTS_:=$(shell find $(UT_DIR) -path '*/.ut' -prune -o \( -name "test_*.py" -o -name "*_test.py" \) -print)
PYTESTS:=$(addprefix PYT/, $(PYTESTS_))
ifneq ($(MAKECMDGOALS),mrproper)
DEPS+=$(patsubst %,$(UT_CACHE)/%.d,$(PYTESTS_))
endif
PASSED_PY=$(patsubst %,$(UT_CACHE)/%.passed,$(PYTESTS_))
ifdef UT_DRY_RUN
FAIL_ACTION:=true
else
# Hmm it seems like we don't want to fail even here, else it prevents watch.sh loop to being run
#FAIL_ACTION:=(rm -f $@ ; exit 1)
FAIL_ACTION:=true
endif
$(UT_CACHE)/%.py.d: $(UT_CACHE)/.gitignore %.py
	$(AT)$(MKDIR) -p $(@D)
	$(AT)$(UTSCRIPTS)/mkpydep.py -t $*.py -o $@ $(PYTO) && touch $(UT_CACHE)/$*.py.passed || $(FAIL_ACTION)
endif

PASSED:=
PASSED+=$(PASSED_PY)
PASSED+=$(PASSED_FAST)
ifneq ("x$(UT_FAST)","x1")
PASSED+=$(PASSED_SLOW)
endif

all: $(PASSED)
dry-run: $(PASSED)

ifdef PYTEST
#test_py: $(PASSED_PY)
TEST_PY_PASSED=$(AT)$(UTSCRIPTS)/mkpydep.py -t $< -o $(UT_CACHE)/$*.py.d $(PYTO) && touch $@
$(UT_CACHE)/%.py.passed: %.py ; $(TEST_PY_PASSED)
else
#test_py:
endif

#test_compiled:
#ifneq ("x$(UT_FAST)","x1")
#	$(AT)echo "FAST TESTS ====================================================="
#endif
#	$(AT)$(MAKE) $(SILENCEMAKE) -f $(CURRENT_MAKEFILE) fast UT_INCLUDES="$(UT_INCLUDES)"
#ifneq ("x$(UT_FAST)","x1")
#	$(AT)echo "SLOW TESTS ====================================================="
#	$(AT)$(MAKE) $(SILENCEMAKE) -f $(CURRENT_MAKEFILE) slow UT_INCLUDES="$(UT_INCLUDES)"
#endif

UT_PROJ:=$(PWD)
watch:
	$(AT)UT_DIR="$(UT_DIR)" UT_PROJ="$(UT_PROJ)" UT_VERBOSE="$(UT_VERBOSE)" UT_FAST="$(UT_FAST)" $(UTSCRIPTS)/watch.sh $(UTARGS)

#fast: test_py $(PASSED_FAST)
#slow: $(PASSED_SLOW)

$(UT_UT):
	$(AT)echo "error: could not find \`$(UT_UT)/\` in \`$(UT_PROJ)\`. Try to init a new ut project there, eg: \`ut init\`"
	$(AT)exit 1
$(UT_CACHE)/.gitignore: $(UT_UT)
	$(AT)$(MKDIR) -p $(@D) && \
	echo "# Created by ut automatically.\n*" > $(UT_CACHE)/.gitignore && \
	echo "Signature: 8a477f597d28d172789f06886806bc55" > $(UT_CACHE)/CACHEDIR.TAG && \
	echo "# This file is a cache directory tag created by ut." >> $(UT_CACHE)/CACHEDIR.TAG && \
	echo "# For information about cache directory tags, see https://bford.info/cachedir/" >> $(UT_CACHE)/CACHEDIR.TAG && \
	echo "# ut cache directory #" > $(UT_CACHE)/README.md && \
	echo "" >> $(UT_CACHE)/README.md && \
	echo "This directory contains data from ut." >> $(UT_CACHE)/README.md && \
	echo "" >> $(UT_CACHE)/README.md && \
	echo "**Do not** commit this to version control." >> $(UT_CACHE)/README.md && \
	echo "" >> $(UT_CACHE)/README.md && \
	echo "See [the docs](https://github.com/nsauzede/ut_/blob/main/README.md) for more information." >> $(UT_CACHE)/README.md

$(UT_CACHE)/%.c.d: $(UT_CACHE)/.gitignore %.c
	$(AT)$(MKDIR) -p $(@D) && \
	(gcc -MM $(CFLAGS) $*.c > $@.orig 2>&1 \
	    || (cat $@.orig | $(CCFILTER) ; $(RM) $@.orig ; false) \
	    && (cat $@.orig | $(UTSCRIPTS)/mkdep.py $(PWD) > $@ ; \
	        $(RM) $@.orig))

$(UT_CACHE)/%.cpp.d: $(UT_CACHE)/.gitignore %.cpp
	$(AT)$(MKDIR) -p $(@D) && \
	(g++ -MM $(CFLAGS) $*.cpp | sed ':a;N;$$!ba;s/\\\n//g' > $@ 2>&1 || (cat $@ ; $(RM) $@ ; false) && (echo "$(UT_CACHE)/$*.cpp.fast.exe $(UT_CACHE)/$*.cpp.slow.exe:" `cat $@ | cut -d':' -f2` > $@))

.INTERMEDIATE: $(EXES)

BUILD_C_EXE=$(AT)($(CC) $*.c -o $@ $(CFLAGS) $(LDFLAGS) > $@.build 2>&1 || (cat $@.build | $(CCFILTER) ; $(RM) $@.build ; false) && $(RM) $@.build)
$(UT_CACHE)/%.c.fast.exe: %.c ; $(BUILD_C_EXE)
$(UT_CACHE)/%.c.slow.exe: %.c ; $(BUILD_C_EXE)

BUILD_CPP_EXE=$(AT)$(CXX) $*.cpp -o $@ $(CXXFLAGS) $(LDFLAGS)
$(UT_CACHE)/%.cpp.fast.exe: %.cpp ; $(BUILD_CPP_EXE)
$(UT_CACHE)/%.cpp.slow.exe: %.cpp ; $(BUILD_CPP_EXE)

TEST_FAST_PASSED=$(AT)./$< $(UTO) && touch $@
$(UT_CACHE)/%.c.fast.passed: CC=$(CC_FAST)
$(UT_CACHE)/%.c.fast.passed: CFLAGS+=-O0
$(UT_CACHE)/%.c.fast.passed: $(UT_CACHE)/%.c.fast.exe ; $(TEST_FAST_PASSED)

TEST_SLOW_PASSED=$(AT)$(VG) ./$< $(UTO) && touch $@
$(UT_CACHE)/%.c.slow.passed: CC=$(CC_SLOW)
$(UT_CACHE)/%.c.slow.passed: CFLAGS+=-g
$(UT_CACHE)/%.c.slow.passed: $(UT_CACHE)/%.c.slow.exe ; $(TEST_SLOW_PASSED)

$(UT_CACHE)/%.cpp.fast.passed: CXX=$(CXX_FAST)
$(UT_CACHE)/%.cpp.fast.passed: CXXFLAGS+=-O0
$(UT_CACHE)/%.cpp.fast.passed: $(UT_CACHE)/%.cpp.fast.exe ; $(TEST_FAST_PASSED)

$(UT_CACHE)/%.cpp.slow.passed: CXX=$(CXX_SLOW)
$(UT_CACHE)/%.cpp.slow.passed: CXXFLAGS+=-g
$(UT_CACHE)/%.cpp.slow.passed: $(UT_CACHE)/%.cpp.slow.exe ; $(TEST_SLOW_PASSED)

mrproper:
	$(AT)$(RM) -rf $(UT_CACHE)

ifneq ($(MAKECMDGOALS),mrproper)
include $(DEPS)
endif
