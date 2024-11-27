#
# UT - Simple and fast Unit Test framework
# Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
# SPDX-License-Identifier: GPL-3.0-or-later
#

CURRENT_MAKEFILE := $(realpath $(firstword $(MAKEFILE_LIST)))
UTPATH:=$(shell dirname $(CURRENT_MAKEFILE))

MKDIR:=mkdir
VALGRIND:=valgrind
ifneq (0, $(shell ($(VALGRIND) /bin/true 2>/dev/null ; echo $$?)))
VALGRIND:=
endif
TCC:=tcc
ifneq (0, $(shell $(TCC) -MM /dev/null > /dev/null 2>&1 ; echo $$?))
TCC:=
endif
GCC:=gcc
ifneq (0, $(shell $(GCC) -MM /dev/null > /dev/null 2>&1 ; echo $$?))
GCC:=
endif
CLANG:=clang
ifneq (0, $(shell $(CLANG) -MM /dev/null > /dev/null 2>&1 ; echo $$?))
CLANG:=
endif

G++:=g++
ifneq (0, $(shell $(G++) -MM /dev/null > /dev/null 2>&1 ; echo $$?))
G++:=
endif
CLANG++:=clang++
ifneq (0, $(shell $(CLANG++) -MM /dev/null > /dev/null 2>&1 ; echo $$?))
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
CCFILTER:=$(UTPATH)/ccolor.sh

-include ut.mk
#$(error UT_INCLUDES=$(UT_INCLUDES))
CFLAGS:=-Wall -Werror -pipe -I$(UTPATH)
CFLAGS+=$(UT_INCLUDES) -I/
CXXFLAGS:=-Wall -Werror -pipe -I$(UTPATH)
CXXFLAGS+=$(UT_INCLUDES) -I/

AT_:=@
AT_1:=
AT:=$(AT_$(V))
SILENCEMAKE_:=-s
#SILENCEMAKE_:=--no-print-directory
SILENCEMAKE_1:=
SILENCEMAKE:=$(SILENCEMAKE_$(V))

C_TESTS:=$(shell find . -name \*.c -exec grep -l "#include .*ut.h" '{}' \;)
CPP_TESTS:=$(shell find . -name \*.cpp -exec grep -l "#include .*ut.h" '{}' \;)
TARGET:=.ut_cache
DEPS:=
DEPS+=$(patsubst %,$(TARGET)/%.d,$(C_TESTS))
DEPS+=$(patsubst %,$(TARGET)/%.d,$(CPP_TESTS))

EXES:=
PASSED_FAST:=
PASSED_SLOW:=

ifdef CC_FAST
EXES+=$(patsubst %,$(TARGET)/%.fast.exe,$(C_TESTS))
PASSED_FAST+=$(patsubst %,$(TARGET)/%.fast.passed,$(C_TESTS))
endif
ifdef CXX_FAST
EXES+=$(patsubst %,$(TARGET)/%.fast.exe,$(CPP_TESTS))
PASSED_FAST+=$(patsubst %,$(TARGET)/%.fast.passed,$(CPP_TESTS))
endif

ifdef CC_SLOW
EXES+=$(patsubst %,$(TARGET)/%.slow.exe,$(C_TESTS))
PASSED_SLOW+=$(patsubst %,$(TARGET)/%.slow.passed,$(C_TESTS))
endif
ifdef CXX_SLOW
EXES+=$(patsubst %,$(TARGET)/%.slow.exe,$(CPP_TESTS))
PASSED_SLOW+=$(patsubst %,$(TARGET)/%.slow.passed,$(CPP_TESTS))
endif

ifdef VALGRIND
VGO:=-q --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --exit-on-first-error=yes --error-exitcode=128
VG:=$(VALGRIND) $(VGO)
endif


ifeq ("x$(UT_VERBOSE)", "x1")
UTO:=-v
endif

all:
ifneq ("x$(UT_FAST)","x1")
	$(AT)echo "FAST TESTS ====================================================="
endif
	$(AT)$(MAKE) $(SILENCEMAKE) -f $(CURRENT_MAKEFILE) fast UT_INCLUDES="$(UT_INCLUDES)"
ifneq ("x$(UT_FAST)","x1")
	$(AT)echo "SLOW TESTS ====================================================="
	$(AT)$(MAKE) $(SILENCEMAKE) -f $(CURRENT_MAKEFILE) slow UT_INCLUDES="$(UT_INCLUDES)"
endif

watch:
	$(AT)(UT_PROJ="$(UT_PROJ)" UT_VERBOSE="$(UT_VERBOSE)" UT_FAST="$(UT_FAST)" $(UTPATH)/watch.sh $(UTARGS))

fast: $(PASSED_FAST)
slow: $(PASSED_SLOW)

$(TARGET):
	$(AT)$(MKDIR) -p $@ && \
	echo "# Created by ut automatically.\n*" > $(TARGET)/.gitignore && \
	echo "Signature: 8a477f597d28d172789f06886806bc55" > $(TARGET)/CACHEDIR.TAG && \
	echo "# This file is a cache directory tag created by ut." >> $(TARGET)/CACHEDIR.TAG && \
	echo "# For information about cache directory tags, see https://bford.info/cachedir/" >> $(TARGET)/CACHEDIR.TAG && \
	echo "# ut cache directory #" > $(TARGET)/README.md && \
	echo "" >> $(TARGET)/README.md && \
	echo "This directory contains data from ut." >> $(TARGET)/README.md && \
	echo "" >> $(TARGET)/README.md && \
	echo "**Do not** commit this to version control." >> $(TARGET)/README.md && \
	echo "" >> $(TARGET)/README.md && \
	echo "See [the docs](https://github.com/nsauzede/ut_/blob/main/README.md) for more information." >> $(TARGET)/README.md

$(TARGET)/%.c.d: $(TARGET) %.c
	$(AT)$(MKDIR) -p $(@D) && \
	($(CC) -MM $(CFLAGS) $*.c > $@.orig 2>&1 || (cat $@.orig | $(CCFILTER) ; $(RM) $@.orig ; false) && (cat $@.orig | sed ':a;N;$$!ba;s/\\\n//g' > $@ ; echo "$(TARGET)/$*.c.fast.exe $(TARGET)/$*.c.slow.exe:" `cat $@ | cut -d':' -f2` > $@ ; $(RM) $@.orig))

$(TARGET)/%.cpp.d: $(TARGET) %.cpp
	$(AT)$(MKDIR) -p $(@D) && \
	($(CXX) -MM $(CFLAGS) $*.cpp | sed ':a;N;$$!ba;s/\\\n//g' > $@ 2>&1 || (cat $@ ; $(RM) $@ ; false) && (echo "$(TARGET)/$*.cpp.fast.exe $(TARGET)/$*.cpp.slow.exe:" `cat $@ | cut -d':' -f2` > $@))

.INTERMEDIATE: $(EXES)

BUILD_C_EXE=$(AT)($(CC) $*.c -o $@ $(CFLAGS) $(LDFLAGS) > $@.build 2>&1 || (cat $@.build | $(CCFILTER) ; $(RM) $@.build ; false) && $(RM) $@.build)
$(TARGET)/%.c.fast.exe: %.c ; $(BUILD_C_EXE)
$(TARGET)/%.c.slow.exe: %.c ; $(BUILD_C_EXE)

BUILD_CPP_EXE=$(AT)$(CXX) $*.cpp -o $@ $(CXXFLAGS) $(LDFLAGS)
$(TARGET)/%.cpp.fast.exe: %.cpp ; $(BUILD_CPP_EXE)
$(TARGET)/%.cpp.slow.exe: %.cpp ; $(BUILD_CPP_EXE)

TEST_FAST_PASSED=$(AT)./$< $(UTO) && touch $@
$(TARGET)/%.c.fast.passed: CC=$(CC_FAST)
$(TARGET)/%.c.fast.passed: CFLAGS+=-O0
$(TARGET)/%.c.fast.passed: $(TARGET)/%.c.fast.exe ; $(TEST_FAST_PASSED)

TEST_SLOW_PASSED=$(AT)$(VG) ./$< $(UTO) && touch $@
$(TARGET)/%.c.slow.passed: CC=$(CC_SLOW)
$(TARGET)/%.c.slow.passed: CFLAGS+=-g
$(TARGET)/%.c.slow.passed: $(TARGET)/%.c.slow.exe ; $(TEST_SLOW_PASSED)

$(TARGET)/%.cpp.fast.passed: CXX=$(CXX_FAST)
$(TARGET)/%.cpp.fast.passed: CXXFLAGS+=-O0
$(TARGET)/%.cpp.fast.passed: $(TARGET)/%.cpp.fast.exe ; $(TEST_FAST_PASSED)

$(TARGET)/%.cpp.slow.passed: CXX=$(CXX_SLOW)
$(TARGET)/%.cpp.slow.passed: CXXFLAGS+=-g
$(TARGET)/%.cpp.slow.passed: $(TARGET)/%.cpp.slow.exe ; $(TEST_SLOW_PASSED)

mrproper:
	$(AT)$(RM) -rf $(TARGET)

ifneq ($(MAKECMDGOALS),mrproper)
include $(DEPS)
endif
