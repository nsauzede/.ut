# UT
![Cle d'UT](res/images/ut.png)

Simple and fast Unit Test framework.

The goal is to accelerate the Test Driven Development methodology.

It is primarily focused on C/C++, under the form of a single header file.
Some scripts also offer some kind of Python support (using pytest under the hood).

# How to use
Include (quoted) `ut.h` single header inside any C or C++ file and add testcases like this:
```C
// Some "a.c" (or "a.cpp") source code file
int foo { return 42; }
...
#include "ut.h"
TESTCASE(Test1)
    TESTMETHOD(test) {
        ASSERT_EQ(42, foo());
    }
...
```
`TESTCASE` registers a kind of TestCase 'class', like with Python `unittest`/`pytest`
and `TESTMETHOD` registers test methods inside such a TestCase 'class'.
Inside such a test method, regular C/C++ code can be written, using `ASSERT*` provided macros
to perform test assertions.

Then the resulting C/C++ test file can be simply built/run as an unit-test executable:
```
$ gcc a.c && ./a.out
```
The executable returns 0 if all test cases/methods pass.

# Convenience tools
Some optional tools are provided, to accelerate the TDD-loop approach: red, green, refactor steps.

1) a `Makefile` is provided to automatically clean/build/test all C/C++ adequate test files
2) a `watch.sh` shell scripts constinuously monitors file-system changes and re-run modified tests
3) an `ut` shell script abstracts these `clean`, `test`, `watch`, ... commands. See `ut --help`
4) `ut` supports Bash auto-completion; register it in `.bashrc` like so: `[ -x ~/ut_/ut ] && . ~/ut_/ut`
(just adapt to where you did install the `ut_` repo on your system)

# Recommended dependencies
It is recommended to install the following dependencies.
- at least one (or more) of these C/C++ compilers: `tcc`, `gcc`, `clang`
- `make`
- `valgrind`
- `inotify-tools`
- `pytest`
