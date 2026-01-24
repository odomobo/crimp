#include <stdio.h>
#include <string.h>

#include "crimpGc.h"
#include "test.h"

int tests_passed = 0;
char const *test_current_name;
bool test_verbose = false;

unitTest_t testCases[TEST_CASE_ARRAY_SIZE];
int testCase_index = 0;

void register_testCase(unitTest_fp fp, char const * name) {
    if (testCase_index >= TEST_CASE_ARRAY_SIZE) {
        fprintf(stderr, "Exceeded %d test cases; expand TEST_CASE_ARRAY_SIZE size\n", TEST_CASE_ARRAY_SIZE);
        exit(1);
    }
    unitTest_t testCase = {
        .fp = fp,
        .name = name
    };
    testCases[testCase_index] = testCase;
    testCase_index++;
}

void run_all_tests() {
    for (int i = 0; i < testCase_index; i++)
    {
        test_current_name = testCases[i].name;
        if (test_verbose) { \
            fprintf(stderr, "[%02d] Beginning test [%s]\n", tests_passed+1, test_current_name);
        }
        testCases[i].fp();
    }
}

int main(int argc, char **argv) {
    // Check for verbose flag
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            test_verbose = true;
            break;
        }
    }

    run_all_tests();

    printf("Passed: %d tests\n", tests_passed);
    return 0;
}
