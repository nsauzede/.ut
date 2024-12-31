#
# UT - Simple and fast Unit Test framework
# Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
# SPDX-License-Identifier: GPL-3.0-or-later
#

CURRENT_MAKEFILE := $(realpath $(firstword $(MAKEFILE_LIST)))
UTPATH:=$(shell dirname `dirname $(CURRENT_MAKEFILE)`)
UTSCRIPTS:=$(UTPATH)/tools
UT_DIR?=.

-include ut*.mk
ifneq (`realpath .`, `realpath $(UT_DIR)`)
-include $(UT_DIR)/ut*.mk
endif

MKDIR:=mkdir
VALGRIND?=valgrind
ifneq (0, $(shell command -v $(VALGRIND) > /dev/null 2>&1 ; echo $$?))
VALGRIND:=
endif
ifneq (x$(UT_NOPY),x1)
PYTEST?=pytest
ifneq (0, $(shell command -v $(PYTEST) > /dev/null 2>&1 ; echo $$?))
PYTEST:=
endif
endif
TCC?=tcc
ifneq (0, $(shell command -v $(TCC) > /dev/null 2>&1 ; echo $$?))
TCC:=
endif
GCC?=gcc
ifneq (0, $(shell command -v $(GCC) > /dev/null 2>&1 ; echo $$?))
GCC:=
endif
CLANG?=clang
ifneq (0, $(shell command -v $(CLANG) > /dev/null 2>&1 ; echo $$?))
CLANG:=
endif

CMAKE?=cmake
ifneq (0, $(shell command -v $(CMAKE) > /dev/null 2>&1 ; echo $$?))
CMAKE:=
endif
GIT?=git
ifneq (0, $(shell command -v $(GIT) > /dev/null 2>&1 ; echo $$?))
GIT:=
endif

GXX?=g++
ifneq (0, $(shell command -v $(GXX) > /dev/null 2>&1 ; echo $$?))
GXX:=
endif
CLANGXX?=clang++
ifneq (0, $(shell command -v $(CLANGXX) > /dev/null 2>&1 ; echo $$?))
CLANGXX:=
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

ifdef GXX
    CXX_FAST:=$(GXX)
else
    ifdef CLANGXX
        CXX_FAST:=$(CLANGXX)
    endif
endif

ifdef CLANGXX
    CXX_SLOW:=$(CLANGXX)
else
    ifdef GXX
        CXX_SLOW:=$(GXX)
    endif
endif
ifdef CC_FAST
CC:=$(CC_FAST)
endif
# Add colored output for C compilers with lack thereof (eg: TCC)
CCFILTER:=$(UTSCRIPTS)/ccolor.sh

UT_UT:=.ut
UT_CACHE:=$(UT_UT)/.cache

UT_3P:=$(UT_UT)/.3p
ifdef CMAKE
ifdef GIT
ifneq (x$(UT_NOGT),x1)
UT_GT:=$(UT_3P)/googletest
GTI:=$(UT_GT)/googletest/include
GTL:=$(UT_GT)/build/lib
endif
endif
endif

CFLAGS+=-Wall -Werror -pipe -I$(UTPATH)/include -DUT_CACHE=\"$(UT_CACHE)\"
CFLAGS+=$(UT_INCLUDES)
CXXFLAGS+=-Wall -Werror -pipe -I$(UTPATH)/include -DUT_CACHE=\"$(UT_CACHE)\"
CXXFLAGS+=$(UT_INCLUDES)

AT_:=@
AT_1:=
AT:=$(AT_$(V))
SILENCEMAKE_:=-s
SILENCEMAKE_1:=
SILENCEMAKE:=$(SILENCEMAKE_$(V))

# Note: the "-path '*/.*/*' -prune" construct below, is used to skip (intermediary) folders whose
# name starts with a dot, eg: like `.3p/` or `.cache/`; it does not however skip files with leading dot.
UT_IHDR:='^\#include.*ut/ut.h'
DEPS:=
ifneq ($(MAKECMDGOALS),mrproper)
UT_C_TESTS:=$(shell find $(UT_DIR) -path '*/.*/*' -prune -o \( -type f -name '*test.c' \) -exec grep -l $(UT_IHDR) '{}' \;)
ifneq (x$(UT_C_TESTS),x)
UT_C_TESTS:=$(shell realpath --relative-to=. $(UT_C_TESTS))
endif
UT_CPP_TESTS:=$(shell find $(UT_DIR) -path '*/.*/*' -prune -o \( -type f -name '*test.cpp' \) -exec grep -l $(UT_IHDR) '{}' \;)
ifneq (x$(UT_CPP_TESTS),x)
UT_CPP_TESTS:=$(shell realpath --relative-to=. $(UT_CPP_TESTS))
endif
DEPS+=$(patsubst %,$(UT_CACHE)/%.d,$(UT_C_TESTS))
DEPS+=$(patsubst %,$(UT_CACHE)/%.d,$(UT_CPP_TESTS))
endif

ifdef UT_GT
GT_IHDR:='^\#include.*gtest/gtest.h'
ifneq ($(MAKECMDGOALS),mrproper)
GT_CC_TESTS:=$(shell find $(UT_DIR) -path '*/.*/*' -prune -o \( -type f -name '*test.cc' \) -exec grep -l $(GT_IHDR) '{}' \;)
ifneq (x$(GT_CC_TESTS),x)
GT_CC_TESTS:=$(shell realpath --relative-to=. $(GT_CC_TESTS))
endif
DEPS+=$(patsubst %,$(UT_CACHE)/%.gt.d,$(GT_CC_TESTS))
endif
endif

EXES:=
PASSED_FAST:=
PASSED_SLOW:=

ifdef CC_FAST
EXES+=$(patsubst %,$(UT_CACHE)/%.fast.exe,$(UT_C_TESTS))
PASSED_FAST+=$(patsubst %,$(UT_CACHE)/%.fast.passed,$(UT_C_TESTS))
endif
ifdef CXX_FAST
EXES+=$(patsubst %,$(UT_CACHE)/%.fast.exe,$(UT_CPP_TESTS))
PASSED_FAST+=$(patsubst %,$(UT_CACHE)/%.fast.passed,$(UT_CPP_TESTS))

EXES+=$(patsubst %,$(UT_CACHE)/%.fast.exe,$(GT_CC_TESTS))
PASSED_FAST+=$(patsubst %,$(UT_CACHE)/%.fast.gt.passed,$(GT_CC_TESTS))
endif

ifdef CC_SLOW
EXES+=$(patsubst %,$(UT_CACHE)/%.slow.exe,$(UT_C_TESTS))
PASSED_SLOW+=$(patsubst %,$(UT_CACHE)/%.slow.passed,$(UT_C_TESTS))
endif
ifdef CXX_SLOW
EXES+=$(patsubst %,$(UT_CACHE)/%.slow.exe,$(UT_CPP_TESTS))
PASSED_SLOW+=$(patsubst %,$(UT_CACHE)/%.slow.passed,$(UT_CPP_TESTS))

EXES+=$(patsubst %,$(UT_CACHE)/%.slow.exe,$(GT_CC_TESTS))
PASSED_SLOW+=$(patsubst %,$(UT_CACHE)/%.slow.gt.passed,$(GT_CC_TESTS))
endif

ifdef VALGRIND
VGO+=-q --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --exit-on-first-error=yes --error-exitcode=128
VG:=$(VALGRIND)
endif

ifdef PYTEST
PYTO:=--tb=short --no-header
ifeq ("x$(UT_VERBOSE)", "x1")
PYTO+=-v
endif
endif

ifeq ("x$(UT_VERBOSE)", "x1")
UTO:=-v
GTO:=
else
GTO:=--gtest_brief=1
endif

ifdef PYTEST
PYTESTS_:=$(shell find $(UT_DIR) -path '*/.*/*' -prune -o \( -name "test_*.py" -o -name "*_test.py" \) -print)
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
ifneq ("x$(UT_SLOW)","x1")
PASSED+=$(PASSED_FAST)
endif
ifneq ("x$(UT_FAST)","x1")
PASSED+=$(PASSED_SLOW)
endif

all: $(PASSED)

dry-run: $(PASSED)

ifdef PYTEST
TEST_PY_PASSED=$(AT)$(UTSCRIPTS)/mkpydep.py -t $< -o $(UT_CACHE)/$*.py.d $(PYTO) && touch $@
$(UT_CACHE)/%.py.passed: %.py ; $(TEST_PY_PASSED)
endif

UT_PROJ:=$(PWD)
watch:
	$(AT)UT_DIR="$(UT_DIR)" UT_PROJ="$(UT_PROJ)" UT_VERBOSE="$(UT_VERBOSE)" UT_FAST="$(UT_FAST)" UT_SLOW="$(UT_SLOW)" $(UTSCRIPTS)/watch.sh $(UTARGS)

$(UT_UT):
	$(AT)echo "error: could not find \`$(UT_UT)/\` in \`$(UT_PROJ)\`. Try to init a new ut project there, eg: \`ut init\`"
	$(AT)exit 1
$(UT_CACHE)/.gitignore: $(UT_UT)
	$(AT)$(MKDIR) -p $(@D) && \
	echo -e "# Created by ut automatically.\n*" > $(UT_CACHE)/.gitignore && \
	echo "Signature: 8a477f597d28d172789f06886806bc55" > $(UT_CACHE)/CACHEDIR.TAG && \
	echo "# This file is a cache directory tag created by ut." >> $(UT_CACHE)/CACHEDIR.TAG && \
	echo "# For information about cache directory tags, see https://bford.info/cachedir/" >> $(UT_CACHE)/CACHEDIR.TAG && \
	echo "# ut cache directory #" > $(UT_CACHE)/README.md && \
	echo "" >> $(UT_CACHE)/README.md && \
	echo "This directory contains data from ut." >> $(UT_CACHE)/README.md && \
	echo "" >> $(UT_CACHE)/README.md && \
	echo "**Do not** commit this to version control." >> $(UT_CACHE)/README.md && \
	echo "" >> $(UT_CACHE)/README.md && \
	echo "See [the docs](https://github.com/nsauzede/.ut/blob/main/README.md) for more information." >> $(UT_CACHE)/README.md

ifdef UT_GT
$(UT_GT):
	$(AT)$(MKDIR) -p $(@D) && \
	(cd $(@D) && $(GIT) clone https://github.com/google/googletest.git && \
	    cd googletest && mkdir build && cd build && cmake .. && make)
endif

$(UT_CACHE)/%.c.d: $(UT_CACHE)/.gitignore %.c
	$(AT)$(MKDIR) -p $(@D) && \
	(gcc -MM $(CFLAGS) $*.c > $@.orig 2>&1 \
	    || (cat $@.orig | $(CCFILTER) ; $(RM) $@.orig ; false) \
	    && (cat $@.orig | $(UTSCRIPTS)/mkdep.py $(PWD) > $@ ; \
	        $(RM) $@.orig))

$(UT_CACHE)/%.cpp.d: $(UT_CACHE)/.gitignore %.cpp
	$(AT)$(MKDIR) -p $(@D) && \
	(g++ -MM $(CXXFLAGS) $*.cpp > $@.orig 2>&1 \
	    || (cat $@.orig | $(CCFILTER) ; $(RM) $@.orig ; false) \
	    && (cat $@.orig | $(UTSCRIPTS)/mkdep.py $(PWD) > $@ ; \
	        $(RM) $@.orig))

ifdef UT_GT
$(UT_CACHE)/%.cc.gt.d: $(UT_CACHE)/.gitignore $(UT_GT) %.cc
	$(AT)$(MKDIR) -p $(@D) && \
	(g++ -MM $(CXXFLAGS) $*.cc > $@.orig 2>&1 \
	    || (cat $@.orig | $(CCFILTER) ; $(RM) $@.orig ; false) \
	    && (cat $@.orig | $(UTSCRIPTS)/mkdep.py $(PWD) > $@ ; \
	        $(RM) $@.orig))
endif

ifeq (x$(V),x1)
UT_KEEP:=1
endif

ifeq (x$(UT_KEEP),x1)
.PRECIOUS: $(EXES)
else
.INTERMEDIATE: $(EXES)
endif

BUILD_C_EXE=$(AT)($(CC) $(CFLAGS) $*.c $(LDFLAGS) -o $@ $(LDLIBS) > $@.build 2>&1 || (cat $@.build | $(CCFILTER) ; $(RM) $@.build ; false) && $(RM) $@.build)
$(UT_CACHE)/%.c.fast.exe: %.c ; $(BUILD_C_EXE)
$(UT_CACHE)/%.c.slow.exe: %.c ; $(BUILD_C_EXE)

BUILD_CPP_EXE=$(AT)$(CXX) $(CXXFLAGS) $*.cpp $(LDFLAGS) -o $@ $(LDLIBS)
$(UT_CACHE)/%.cpp.fast.exe: %.cpp ; $(BUILD_CPP_EXE)
$(UT_CACHE)/%.cpp.slow.exe: %.cpp ; $(BUILD_CPP_EXE)

BUILD_CC_EXE=$(AT)$(CXX) $(CXXFLAGS) $*.cc $(LDFLAGS) -o $@ $(LDLIBS)
$(UT_CACHE)/%.cc.fast.exe: %.cc ; $(BUILD_CC_EXE)
$(UT_CACHE)/%.cc.slow.exe: %.cc ; $(BUILD_CC_EXE)

TEST_FAST_PASSED=$(AT)LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./$< $(UTO) && touch $@
$(UT_CACHE)/%.c.fast.passed: CC=$(CC_FAST)
$(UT_CACHE)/%.c.fast.passed: CFLAGS+=-O0
$(UT_CACHE)/%.c.fast.passed: $(UT_CACHE)/%.c.fast.exe ; $(TEST_FAST_PASSED)

ifdef VG
VG_:=$(VG) $(VGO)
endif
TEST_SLOW_PASSED=$(AT)LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) $(VG_) ./$< $(UTO) && touch $@
$(UT_CACHE)/%.c.slow.passed: CC=$(CC_SLOW)
$(UT_CACHE)/%.c.slow.passed: CFLAGS+=-g
$(UT_CACHE)/%.c.slow.passed: $(UT_CACHE)/%.c.slow.exe ; $(TEST_SLOW_PASSED)

$(UT_CACHE)/%.cpp.fast.passed: CXX=$(CXX_FAST)
$(UT_CACHE)/%.cpp.fast.passed: CXXFLAGS+=-O0
$(UT_CACHE)/%.cpp.fast.passed: $(UT_CACHE)/%.cpp.fast.exe ; $(TEST_FAST_PASSED)

$(UT_CACHE)/%.cc.fast.gt.passed: CXX=$(CXX_FAST)
$(UT_CACHE)/%.cc.fast.gt.passed: CXXFLAGS+=-O0 -I$(GTI)
$(UT_CACHE)/%.cc.fast.gt.passed: LDFLAGS+=-L$(GTL) -lgtest -lgtest_main
$(UT_CACHE)/%.cc.fast.gt.passed: UTO=$(GTO)
$(UT_CACHE)/%.cc.fast.gt.passed: $(UT_CACHE)/%.cc.fast.exe ; $(TEST_FAST_PASSED)

$(UT_CACHE)/%.cpp.slow.passed: CXX=$(CXX_SLOW)
$(UT_CACHE)/%.cpp.slow.passed: CXXFLAGS+=-O0 -g
$(UT_CACHE)/%.cpp.slow.passed: $(UT_CACHE)/%.cpp.slow.exe ; $(TEST_SLOW_PASSED)

$(UT_CACHE)/%.cc.slow.gt.passed: CXX=$(CXX_SLOW)
$(UT_CACHE)/%.cc.slow.gt.passed: CXXFLAGS+=-O0 -g -I$(GTI)
$(UT_CACHE)/%.cc.slow.gt.passed: LDFLAGS+=-L$(GTL) -lgtest -lgtest_main
$(UT_CACHE)/%.cc.slow.gt.passed: UTO=$(GTO)
$(UT_CACHE)/%.cc.slow.gt.passed: $(UT_CACHE)/%.cc.slow.exe ; $(TEST_SLOW_PASSED)

tools/inww.exe: CFLAGS:=-Wall -Werror -O3
tools/inww.exe: tools/inww.c
	$(AT)gcc $(CFLAGS) -o $@ $< $(LDFLAGS)
	$(AT)strip --strip-all $@

mrproper:
	$(AT)$(RM) -rf `realpath $(UT_CACHE)/$(UT_DIR)`

ifneq ($(MAKECMDGOALS),mrproper)
include $(DEPS)
endif
