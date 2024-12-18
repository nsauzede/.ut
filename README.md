# UT
[![Build Status][WorkflowBadgeLinux]][WorkflowUrl] [![Build Status][WorkflowBadgeWindows]][WorkflowUrl]

![Cle d'UT](res/images/ut.png)

## Simple And Fast Unit Test Framework.

The goal is to accelerate the Test Driven Development methodology.

It supports `Linux` and `Windows` (msys2), and is primarily focused on `C/C++`, under the form of a single header file,
and is meant to be completely independent to the user's existing build system,
using a set a convenience tools to automate the building and running of the tests.
It also supports any `googletest` conformant source/test files (googletest will be automatically fetched/installed if cmake and git are available).

Some tools also offer some kind of `Python` support (using `pytest` under the hood).

### Quick start for the impatient
1. Clone UT at the root of your existing project:

`$ git clone https://github.com/nsauzede/.ut`

That's it! Everything is setup, tests can be written and run.

2. Add tests:

Include `ut/ut.h` inside any C or C++ source/test file (must be named '*test.{c,cpp}' for automatic detection by `ut` tool) and add testcases like this:
```C
// Some "foo_test.c" source/test file
int foo() { return 42; }
#include <ut/ut.h>
TESTCASE(Test_foo_behaviour)
    TESTMETHOD(test_foo_returns_the_answer) {
        ASSERT_EQ(42, foo());
    }
```
```C++
// Some "foo_test.cpp" source/test file
class Foo { public: int foo() { return 42; } } foo;
#include <ut/ut.h>
TESTCASE(Test_foo_behaviour)
    TESTMETHOD(test_foo_returns_the_answer) {
        ASSERT_EQ(42, foo.foo());
    }
```
3. Run tests:
```
$ .ut/ut --fast -v retest
============================== test session starts ===============================
platform win32 -- ut 0.0.10, language C, GCC 14.2.0 -- C:\msys64\tmp\.ut\.cache\test.c.fast.exe
cachedir: .ut/.cache
rootdir: C:\msys64\tmp
collected 1 items

test.c::test_foo_returns_the_answer PASSED                                 [100%]

=============================== 1 passed in 0.00s ================================
============================== test session starts ===============================
platform win32 -- ut 0.0.10, language C++, GCC 14.2.0 -- C:\msys64\tmp\.ut\.cache\foo_test.cpp.fast.exe
cachedir: .ut/.cache
rootdir: C:\msys64\tmp
collected 1 items

foo_test.cpp::test_foo_returns_the_answer PASSED                           [100%]

=============================== 1 passed in 0.00s ================================
```
4. ...
5. _Profit!_

If TCC, GCC, CLANG, VALGRIND (linux only), are available, they will all be used automatically:
- fast tests try to use `tcc` (or `gcc`, or `clang`) and `g++` (or `clang++`)
- slow tests try to use `clang` (or `gcc`, or `tcc`) and `clang++` (or `g++`) and `valgrind`

The `test` command above automatically builds and tests all/only the last modified source/test files.
The `--fast` and `-v` options are optional, and select only fast tests, with increased verbosity.
Consult `.ut/ut --help` for more information.

To systematically rebuild/retest everything unconditionally, use `retest`.

To enter a typical TDD Red-Green-Refactor loop, use `watch`:
```
$ .ut/ut watch
```
=> From now on, each you modify a source/test file (eg: within VSCode), all related tests will get automatically rebuilt/retested.


Note that the tests can be put in separate files than the implementation.
In that case, all that is needed, is to include the source file at the top of the test file, like so:
```C
// Some "foo_test.c" (or eg: "foo_test.cpp") test file
#include "path/to/foo.c"          // or #include "path/to/foo.cpp"

#include <ut/ut.h>
TESTCASE(Test_foo_behaviour)
    TESTMETHOD(test_foo_returns_the_answer) {
        ASSERT_EQ(42, foo());
    }
...
```


`TESTCASE` registers a kind of TestCase 'class', like with Python `unittest`/`pytest`
and `TESTMETHOD` registers a test method inside such a TestCase 'class'.
Inside such a test method, regular C/C++ code can be written, using `ASSERT*` provided macros
to perform test assertions.

Note that, for simple tests cases, the `TESTCASE` construct can be ommitted, thus only `TESTMETHOD` have to be used, for less boilerplate code.
This can be useful eg: for quickly prototyping an idea with the following minimalistic source/test file:
```C
// Some "foo_test.c" (or "foo_test.cpp") source source/test file
int foo { return 42; }
...
#include <ut/ut.h>
TESTMETHOD(test_foo_returns_the_answer) {
    ASSERT_EQ(42, foo());
}
...
```
The only caveat, in that case, is that all such defined `TESTMETHOD` symbols (`test_foo_returns_the_answer` in that case) must be unique, whereas they could be duplicates when registered in different `TESTCASE` like previously.


Note that in fact any C/C++ file (even not named `*test.{c,cpp}`) can also be manually built/run as an unit-test executable like so:
```
$ gcc a.c -o a -I.ut/include && ./a
a.c .                                                            1 passed in 0.00s
```
The executable returns 0 if all test cases/methods pass.

### Detailed information (aka old documentation)
Some optional tools/scripts are provided by UT, to accelerate the TDD-loop approach: red-green-refactor steps.

1) a `Makefile` is provided to automatically clean/build/test all C/C++ adequate test files
2) a `watch.sh` shell script constinuously monitors file-system changes and re-run modified tests
3) an `ut` shell script abstracts these `clean`, `test`, `watch`, ... commands. See `ut --help`
4) `ut` supports Bash auto-completion; register it in `.bashrc` like so: `[ -x ~/.ut/ut ] && . ~/.ut/ut`
(just adapt to where you did install UT repo on your system)

Once you cloned the UT git directory somewhere, and added its location in your PATH, you don't even need to clone it again
in all your projects, you just have to use the `ut init` command (akin to eg: `git init`) to initialize them.
It will only create a mostly empty `.ut` directory at the root of your project, which will contain eg: test cache datas.

How (easy it is) to use `ut` tool?
First you have to initialize the ut project root directory (only once!).
This is where the cache will be stored, and it defines the subtree that will be searched for tests to run.
```shell
$ cd <to your future ut project root>
$ ut init
```
Then, you can run the tests, from anywhere below or at your ut project root:
```shell
$ cd <anywhere below or at ut project root>
$ ut test
< Python and/or C/C++ tests output>
...
$
```
If all tests passed, then an immediate `test` rerun (ie: without changing the sources) should not output anything:
```shell
$ cd <anywhere below or at ut project root>
$ ut test
$
```
That's expected, because all tests passed, and source files/tests have not changed.
If needed, it is possible to force the tests to re-run, either by cleaning the cache:
```shell
$ cd <anywhere below or at ut project root>
$ ut clean
$ ut test
< Python and/or C/C++ tests output>
...
$
```
Or by using the `retest` command that just does `clean`+`test`:
```shell
$ cd <anywhere below or at ut project root>
$ ut retest
< Python and/or C/C++ tests output>
...
$
```

Note that a handy feature allows to only run tests below a certain subdirectory (to not run any tests outside of it).
Eg: if there are two test subdirs `tests1/` and `tests1/` below the ut project root, then the following will only re-run tests from `test1/`:
```shell
$ cd <to ut project root>
$ ut retest tests1
< Python and/or C/C++ tests output from tests1 only >
...
$
```


# Dependencies
It is required to install the following dependencies:
- `make` (4.3+), `gcc` (13.2+)
- `python` (3.9+)
- `inotify-tools` (linux only)

It is also recommended to install those too:
- `git`, `cmake`, `pytest`
- `tcc`, `clang`
- `valgrind` (linux only)

[WorkflowBadgeLinux]: https://github.com/nsauzede/.ut/workflows/Linux/badge.svg
[WorkflowBadgeWindows]: https://github.com/nsauzede/.ut/workflows/Windows/badge.svg
[WorkflowUrl]: https://github.com/nsauzede/.ut/commits/main
