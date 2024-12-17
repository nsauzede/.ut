#ifndef UT_H__
#define UT_H__

/*
* UT - Simple and fast Unit Test framework
* Copyright (C) 2024 Nicolas Sauzede (nicolas.sauzede@gmail.com)
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include <setjmp.h>

#ifdef _WIN32
#include <direct.h>     // _getcwd
#include <io.h>         // _pipe, _dup, _dup2
#include <fcntl.h>      // _O_TEXT, _O_BINARY
#include <windows.h>    // PeekNamedPipe
#else
#include <unistd.h>     // getcwd
#include <sys/ioctl.h>  // ioctl
#endif

#define UT_VERSION "0.0.10"

#define CTOR __attribute((constructor))
#define TRY() (setjmp(UT.jmpbuf) == 0)
#define THROW() do{longjmp(UT.jmpbuf, 1);}while(0)
#define RED "\x1b[0;31m"
#define BRED "\x1b[1;31m"
#define GRN "\x1b[0;32m"
#define BGRN "\x1b[1;32m"
#define YEL "\x1b[0;33m"
#define CYAN "\x1b[1;36m"
#define BWHITE "\x1b[1;37m"
#define BLUE "\x1b[1;34m"
#define NRM "\x1b[0m"
#define SETCLASS(_cls) UT.cls=#_cls
#define JOIN_(x, y) x##y
#define JOIN(x, y) JOIN_(x, y)
#define UMET(met) JOIN(JOIN(L,__LINE__),_##met)
#define ADDTEST(met, umet, test) do{ut_add(__FILE__, #met, STRINGIFY(umet), umet, test);}while(0)
#define DEFCTOR(met) static void CTOR UMET(met##_ctor)(){ADDTEST(met, UMET(met), &UMET(met##_s));}
#define TESTCASE(cls) static void CTOR cls##_ctor() { SETCLASS(cls); }
#define DECLT(met) static struct UT_s UMET(met##_s)
#define DECLM(met) static void UMET(met)()
#define TESTMETHOD(met) DECLM(met); DECLT(met); DEFCTOR(met) DECLM(met)
#define ASSERT(expr) do{if(!ut_assert(__FILE__,__LINE__,__func__,#expr,expr))return;}while(0)
#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define ut_ssize_t long
#ifndef FIONREAD
#define FIONREAD 0x541B
#endif
#define ut_getcwd _getcwd
#define ut_pipe(fdp) _pipe(fdp, 1024, _O_BINARY)
#define ut_dup(fd) _dup(fd)
#define ut_dup2(fd1, fd2) _dup2(fd1, fd2)
#define ut_read(fd, buf, size) _read(fd, buf, size)
#define ut_close(fd) _close(fd)
#define ut_ioctl(fd, op, ptr) ut_fake_ioctl(fd, op, ptr)
#else
#define ut_getcwd getcwd
#define ut_ssize_t ssize_t
#define ut_pipe pipe
#define ut_dup dup
#define ut_dup2 dup2
#define ut_read read
#define ut_close close
#define ut_ioctl ioctl
#endif

#if defined(_WIN32) && (defined(__clang__) || defined(__TINYC__))
#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC UT_CLOCK_MONOTONIC
#define clock_gettime ut_clock_gettime
#define timespec ut_timespec
typedef enum {UT_CLOCK_MONOTONIC} ut_clockid_t;
typedef struct ut_timespec{int tv_sec, tv_nsec;} ut_timespec_t;
static int
ut_clock_gettime(ut_clockid_t clockid, struct ut_timespec *tp) {
    if (tp) {
        tp->tv_sec = tp->tv_nsec = 0;
    }
    return 0;
}
#endif
#endif

struct UT_cap_s {
    int fileno, original_fd, std_pipe[2];
};
static struct UT_s {
    const char *cls, *met, *umet;
    const char *file, *func, *expr_str;
    int line, expr;
    void (*ptr)();
    jmp_buf jmpbuf;
    int disabled;
    struct UT_cap_s cap_stdout, cap_stderr;
    struct UT_s *next;
} UT, *ut_last, *ut_curr;

#ifdef _WIN32
int ut_fake_ioctl(int fd, unsigned long op, int *ptr) {
    DWORD val = 0;
    if (op == FIONREAD) {
        PeekNamedPipe((HANDLE)_get_osfhandle(fd), NULL, 0, NULL, &val, NULL);
    }
    if (ptr) *ptr = val;
    return 0;
}
#endif
void ut_cap_init(struct UT_cap_s *cap, int fileno) {
    cap->fileno = fileno;
    cap->original_fd = -1;
    cap->std_pipe[0] = -1;
    cap->std_pipe[1] = -1;
}
void ut_cap_start(struct UT_cap_s *cap) {
    cap->original_fd = ut_dup(cap->fileno);
    ut_pipe(cap->std_pipe);
    ut_dup2(cap->std_pipe[1], cap->fileno);
    ut_close(cap->std_pipe[1]);
}
void ut_cap_stop(struct UT_cap_s *cap) {
    if (cap->original_fd != -1) {
        ut_dup2(cap->original_fd, cap->fileno);
        ut_close(cap->original_fd);
    }
}
int ut_cap_bytes(struct UT_cap_s *cap) {
    int bytes = 0;
    if (cap->std_pipe[0] != -1) {
        ut_ioctl(cap->std_pipe[0], FIONREAD, &bytes);
    }
    return bytes;
}
void ut_cap_flush(struct UT_cap_s *cap) {
    while (1) {
        char buf[1024];
        ut_ssize_t count = ut_read(cap->std_pipe[0], buf, sizeof(buf) - 1);
        if (count <= 0)break;
        buf[count] = 0;
        printf("%s", buf);
    }
    ut_close(cap->std_pipe[0]);
}
void ut_add(const char *file, const char *met, const char *umet, void (*ptr)(), struct UT_s *test) {
    if (UT.cls && strncmp(UT.cls, "Test", 4))return;
    if (!met || strncmp(met, "test", 4))return;
    if (!ut_last)ut_last = &UT;
    ut_last->next = test;
    ut_last = test;
    test->cls = UT.cls;
    test->met = met;
    test->umet = umet;
    test->file = file;
    test->ptr = ptr;
}
int ut_assert(const char *file, int line, const char *func, const char *expr_str, int expr) {
    if (!expr) {
        ut_curr->file = file;
        ut_curr->line = line;
        ut_curr->func = !strcmp(func, ut_curr->umet) ? ut_curr->met : func;
        ut_curr->expr_str = expr_str;
        ut_curr->expr = expr;
        THROW();
    }
    return expr;
}
int ut_get_term_width() {
    int width = 80;
#if !(defined(_WIN32) && defined(__clang__))
    if (getenv("TERM")) {
        char buf[1024] = "tput cols";
        FILE *p = popen(buf, "r");
        if (p) {
            if (fgets(buf, sizeof(buf), p)) {
                sscanf(buf, "%d", &width);
            }
            pclose(p);
        }
    }
#endif
    return width;
}
void ut_print_centered(int columns, const char *color, const char sep, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(0, 0, fmt, ap);
    va_end(ap);
    va_start(ap, fmt);
    int rem = columns - n - 2;
    int left = rem / 2;
    int right = rem - left;
    printf("%s", color);
    for (int i = 0; i < left; i++, printf("%c", sep));
    printf(" ");
    vprintf(fmt, ap);
    va_end(ap);
    printf(" %s", color);
    for (int i = 0; i < right; i++, printf("%c", sep));
    printf(NRM"\n");
}
void ut_print_right(int columns, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(0, 0, fmt, ap);
    va_end(ap);
    va_start(ap, fmt);
    int rem = columns - n - 1;
    for (int i = 0; i < rem; i++, printf(" "));
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}
int ut_run_test(struct UT_s *ut) {
    int failed = 0;
    ut_curr = ut;
    ut_curr->expr = 1;
    if (TRY()) {
        ut->ptr();
    }
    failed = !ut_curr->expr;
    return failed;
}
void ut_help(const char *prog) {
    printf("usage: %s [options]\n", prog);
    printf("\n");
    printf("Options:\n");
    printf("    --help          This help\n");
    printf("    -s              Do not capture tests stdout/stderr\n");
    printf("    -v              Increase verbosity\n");
    printf("    -q              Decrease verbosity\n");
}
int ut_main_(int argc, char *argv[]) {
    int verbosity = 2;
    int nocap = 0;
    int arg = 1;

    while (arg < argc) {
        if (!strcmp(argv[arg], "--help")) {
            ut_help(argv[0]);
            return 0;
        } else if (!strcmp(argv[arg], "-v")) {
            verbosity++;
        } else if (!strcmp(argv[arg], "-q")) {
            verbosity--;
        } else if (!strcmp(argv[arg], "-s")) {
            nocap = 1;
        }
        arg++;
    }
    int failed = 0;
    int passed = 0;
    int width = ut_get_term_width();
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    int ntests = 0;
    struct UT_s *ut = UT.next;
    while (ut) {
        ntests++;
        ut = ut->next;
    }
    if (verbosity >= 3) {
    ut_print_centered(width, BWHITE, '=', "test session starts");
    const char *platform =
#ifdef __linux__
    "linux"
#elif defined _WIN32
    "win32"
#else
    "unknown"
#endif
    ;
    struct { const char *name; int major, minor, extra; } compiler = {
#ifdef __clang__
    "Clang", __clang_major__, __clang_minor__, __clang_patchlevel__
#elif defined(__TINYC__)
    "TinyCC", __TINYC__ / 10000, (__TINYC__ % 10000) / 100, __TINYC__ % 100
#elif defined(__GNUC__)
    "GCC", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__
#else
    "unknown", 0, 0, 0
#endif
    };
    const char *language =
#ifdef __cplusplus
    "C++"
#else
    "C"
#endif
    ;
    const char *ut_version = UT_VERSION;
    printf("platform %s -- ut %s, language %s, %s %d.%d.%d -- %s\n", platform, ut_version, language, compiler.name, compiler.major, compiler.minor, compiler.extra, argv[0]);
#ifdef UT_CACHE
    const char *ut_cache = UT_CACHE;
    printf("cachedir: %s\n", ut_cache);
#endif
    char *cwd = ut_getcwd(NULL, 0);
    printf("rootdir: %s\n", cwd);
    free(cwd);
    printf(BWHITE "collected %d items\n" NRM, ntests);
    printf("\n");
    }
    const char *last_file = 0;
    int vcolumn = 0;
    ut = UT.next;
    struct timespec ts0, ts1;
    clock_gettime(CLOCK_MONOTONIC, &ts0);
    while (ut) {
        if (ut->disabled){
            ut = ut->next;
            continue;
        }
        if (verbosity>=3) {
            if (ut->cls) {
                vcolumn += printf("%s::%s::%s ", ut->file, ut->cls, ut->met);
            } else {
                vcolumn += printf("%s::%s ", ut->file, ut->met);
            }
        } else if (!last_file || strcmp(last_file, ut->file)) {
            if (verbosity >= 1) {
                vcolumn += printf("%s ", ut->file);
            }
        }
        if (!last_file || strcmp(last_file, ut->file)) {
            last_file = ut->file;
        }
        if (!nocap) {
            ut_cap_init(&ut->cap_stdout, STDOUT_FILENO);
            ut_cap_init(&ut->cap_stderr, STDERR_FILENO);
            ut_cap_start(&ut->cap_stdout);
            ut_cap_start(&ut->cap_stderr);
        }
        int failure = ut_run_test(ut);
        if (!nocap) {
            ut_cap_stop(&ut->cap_stdout);
            ut_cap_stop(&ut->cap_stderr);
        }
        if (failure) {
            failed++;
            if (verbosity > 0) {
                vcolumn += printf(RED "%s" NRM, verbosity>=3 ? "FAILED" : "F") - strlen(RED) - strlen(NRM);
            }
        } else {
            passed++;
            if (verbosity > 0) {
                vcolumn += printf(GRN "%s" NRM, verbosity>=3 ? "PASSED" : ".") - strlen(GRN) - strlen(NRM);
            }
        }
        ut = ut->next;
        int print_trailer = 0;
        if ((verbosity >= 3) || !ut || (last_file && strcmp(last_file, ut->file))) {
            print_trailer = 1;
        }
        if ((verbosity >= 3) && print_trailer) {
            if (nocap){
                printf("\n");
            } else {
                int pct = 100 * (failed + passed) / ntests;
                const char *color = !failed ? GRN : RED;
                int vcolumns = width - vcolumn + strlen(color) + strlen(NRM);
                ut_print_right(vcolumns, "%s[%3d%%]" NRM, color, pct);
            }
            vcolumn = 0;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    double t = ts1.tv_sec - ts0.tv_sec + ((double)ts1.tv_nsec - ts0.tv_nsec) / 1000000000;
    if (!failed) {
        if (passed) {
            if (verbosity >= 3) {
                printf("\n");
            } else if (verbosity >= 1) {
                printf(" ");
            }
            if (verbosity >= 3) {
                ut_print_centered(width + strlen(BGRN) + strlen(GRN), GRN, '=', BGRN "%d passed" GRN " in %.02fs", passed, t);
            } else if (verbosity >= 1) {
#if 0
                printf(BGRN "%d passed" GRN " in %.02fs\n" NRM, passed, t);
#else
                int vcolumns = width - vcolumn + strlen(BGRN) + strlen(GRN) + strlen(NRM);
                ut_print_right(vcolumns, BGRN "%d passed" GRN " in %.02fs" NRM, passed, t);
#endif
            }
            return 0;
        } else {
            if (verbosity >= 2) {
                ut_print_centered(width, YEL, '=', "no tests ran in %.02fs", t);
            } else {
                if (verbosity >= 1) {
                    printf("%s ", argv[0]);
                    printf(YEL "no tests ran in %.02fs\n" NRM, t);
                }
            }
            return 5; // like pytest
        }
    }
    printf("\n");
    ut_print_centered(width, NRM, '=', "FAILURES");
    ut = UT.next;
    while (ut) {
        if (ut->disabled){
            ut = ut->next;
            continue;
        }
        if (!ut->expr) {
            if (ut->cls) {
                ut_print_centered(width, BRED, '_', "%s.%s", ut->cls, ut->met);
            } else {
                ut_print_centered(width, BRED, '_', "%s", ut->met);
            }
            printf(BRED "%s" NRM":%d: in %s\n", ut->file, ut->line, ut->func);
            printf("\t" BLUE "ASSERT" NRM "(%s)\n", ut->expr_str);
            if (ut_cap_bytes(&ut->cap_stdout)) {
                ut_print_centered(width, NRM, '-', "Captured stdout call");
                ut_cap_flush(&ut->cap_stdout);
            }
            if (ut_cap_bytes(&ut->cap_stderr)) {
                ut_print_centered(width, NRM, '-', "Captured stderr call");
                ut_cap_flush(&ut->cap_stderr);
            }
        }
        ut = ut->next;
    }
    ut_print_centered(width, CYAN, '=', "short test summary info");
    ut = UT.next;
    while (ut) {
        if (ut->disabled){
            ut = ut->next;
            continue;
        }
        if (!ut->expr) {
            if (ut->cls) {
                printf(RED "FAILED" NRM " %s:%d::" BWHITE "%s::%s" NRM " - ASSERT(%s)\n", ut->file, ut->line, ut->cls, ut->met, ut->expr_str);
            } else {
                printf(RED "FAILED" NRM " %s:%d::" BWHITE "%s" NRM " - ASSERT(%s)\n", ut->file, ut->line, ut->met, ut->expr_str);
            }
        }
        ut = ut->next;
    }
    if (passed) {
        if (verbosity >= 2) {
            ut_print_centered(width + strlen(BRED) + strlen(NRM) + strlen(GRN) + strlen(RED), RED, '=', BRED "%d failed" NRM ", " GRN "%d passed" RED " in %.02fs", failed, passed, t);
        } else {
            printf(BRED "%d failed" NRM ", " GRN "%d passed" RED " in %.02fs\n" NRM, failed, passed, t);
        }
    } else {
        if (verbosity >= 2) {
            ut_print_centered(width + strlen(BRED) + strlen(RED), RED, '=', BRED "%d failed" RED " in %.02fs", failed, t);
        } else {
            printf(BRED "%d failed" RED " in %.02fs\n" NRM, failed, t);
        }
    }
    return 1;
}
int ut_assert_eq_ptr(const char *file, int line, const char *func, const char *expr_str, void *lhs, void *rhs) {
    int expr = lhs == rhs;
    if (!expr) {
        fprintf(stderr, "%s:%d: AssertionError (Pointer)\n", file, line);
        fprintf(stderr, ">\tASSERT_EQ(%s)\n", expr_str);
        fprintf(stderr, "E\tASSERT_EQ(%p, %p)\n", lhs, rhs);
        ut_assert(file, line, func, expr_str, expr);
    }
    return expr;
}
int ut_assert_neq_ptr(const char *file, int line, const char *func, const char *expr_str, void *lhs, void *rhs) {
    int expr = lhs != rhs;
    if (!expr) {
        fprintf(stderr, "%s:%d: AssertionError (Pointer)\n", file, line);
        fprintf(stderr, ">\tASSERT_NEQ(%s)\n", expr_str);
        fprintf(stderr, "E\tASSERT_NEQ(%p, %p)\n", lhs, rhs);
        ut_assert(file, line, func, expr_str, expr);
    }
    return expr;
}
int ut_assert_eq_int(const char *file, int line, const char *func, const char *expr_str, int lhs, int rhs) {
    int expr = lhs == rhs;
    if (!expr) {
        fprintf(stderr, "%s:%d: AssertionError (Integer)\n", file, line);
        fprintf(stderr, ">\tASSERT_EQ(%s)\n", expr_str);
        fprintf(stderr, "E\tASSERT_EQ(%d, %d)\n", lhs, rhs);
        ut_assert(file, line, func, expr_str, expr);
    }
    return expr;
}
int ut_assert_eq_str(const char *file, int line, const char *func, const char *expr_str, const char* lhs, const char* rhs) {
    int expr = !strcmp(lhs, rhs);
    if (!expr) {
        fprintf(stderr, "%s:%d: AssertionError (String)\n", file, line);
        fprintf(stderr, ">\tASSERT_EQ(%s)\n", expr_str);
        fprintf(stderr, "E\tASSERT_EQ(\"%s\", \"%s\")\n", lhs, rhs);
        ut_assert(file, line, func, expr_str, expr);
    }
    return expr;
}
#ifndef UT_NO_MAIN
int main(int argc, char *argv[]) { return ut_main_(argc, argv); }
#define main ut_main
#endif
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
#define ASSERT_EQ_(va_args, lhs, rhs) ut_assert_eq_(__FILE__,__LINE__,__func__,va_args, lhs, rhs)
#define ASSERT_NEQ_(va_args, lhs, rhs) ut_assert_neq_(__FILE__,__LINE__,__func__,va_args, lhs, rhs)
int ut_assert_eq_(const char *file, int line, const char *func, const char *va_args, int lhs, int rhs) { return ut_assert_eq_int(file, line, func, va_args, lhs, rhs); }
int ut_assert_eq_(const char *file, int line, const char *func, const char *va_args, const char* lhs, const char* rhs) { return ut_assert_eq_str(file, line, func, va_args, lhs, rhs); }
int ut_assert_eq_(const char *file, int line, const char *func, const char *va_args, void* lhs, void* rhs) { return ut_assert_eq_ptr(file, line, func, va_args, lhs, rhs); }
int ut_assert_neq_(const char *file, int line, const char *func, const char *va_args, void* lhs, void* rhs) { return ut_assert_neq_ptr(file, line, func, va_args, lhs, rhs); }
#else
#define ASSERT_EQ_(va_args, lhs, rhs) _Generic((lhs), \
    int: ut_assert_eq_int, \
    void*: ut_assert_eq_ptr, \
    const char*: ut_assert_eq_str)(__FILE__,__LINE__,__func__,va_args,lhs, rhs)
#define ASSERT_NEQ_(va_args, lhs, rhs) _Generic((lhs), \
    void*: ut_assert_neq_ptr)(__FILE__,__LINE__,__func__,va_args,lhs, rhs)
#endif
#define ASSERT_EQ(...) ASSERT_EQ_(#__VA_ARGS__, __VA_ARGS__)
#define ASSERT_NEQ(...) ASSERT_NEQ_(#__VA_ARGS__, __VA_ARGS__)
#endif/*UT_H__*/
