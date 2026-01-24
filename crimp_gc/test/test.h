#ifndef TEST_H
#define TEST_H

// needed for macros
#include <stdlib.h> // IWYU pragma: keep
#include <stdio.h> // IWYU pragma: keep
#include <stdbool.h>

#include "crimpGc_internals.h"


extern int tests_passed;
extern char const *test_current_name;
extern bool test_verbose;

typedef void (*unitTest_fp)();

typedef struct unitTest_t {
    unitTest_fp fp;
    char const * name;
} unitTest_t;

void register_testCase(unitTest_fp testCase, char const * name);

#define TEST_CASE_ARRAY_SIZE 1024
extern unitTest_t testCases[TEST_CASE_ARRAY_SIZE];
extern int testCase_index;

#if defined(_MSC_VER)
    #pragma section(".CRT$XCU",read)
    #define INITIALIZER2_(f,p) \
        static void f(void); \
        __declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f; \
        __pragma(comment(linker,"/include:" p #f "_")) \
        static void f(void)
    #ifdef _WIN64
        #define INITIALIZER(f) INITIALIZER2_(f,"")
    #else
        #define INITIALIZER(f) INITIALIZER2_(f,"_")
    #endif
#else
    #define INITIALIZER(f) \
        static void f(void) __attribute__((constructor)); \
        static void f(void)
#endif

#define TESTCASE(testName) \
    static void testName(); \
    INITIALIZER(testName##_registration) { \
        register_testCase(&testName, #testName); \
    } \
    static void testName()


#define test_assert(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "Test [%s] failed: ( %s ) in %s on line #%d\n", test_current_name, #condition, __FILE__, __LINE__); \
        exit(20); \
    } \
} while (0)

#endif // TEST_H