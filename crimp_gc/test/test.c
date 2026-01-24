#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "crimpGc_internals.h"
#include "test.h"

char const *test_current_name;

unitTest_t testCases[TEST_CASE_ARRAY_SIZE];
int testCase_index = 0;

void register_testCase(unitTest_fp fp, char const * name, int expectedReturn) {
    if (testCase_index >= TEST_CASE_ARRAY_SIZE) {
        fprintf(stderr, "Exceeded %d test cases; expand TEST_CASE_ARRAY_SIZE size\n", TEST_CASE_ARRAY_SIZE);
        exit(1);
    }
    unitTest_t testCase = {
        .fp = fp,
        .name = name,
        .expectedReturn = expectedReturn
    };
    testCases[testCase_index] = testCase;
    testCase_index++;
}

void print_usage(char *commandSelf)
{
    fprintf(stderr, "Usage: %s [list|runCase <index>]\n", commandSelf);
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        if (strcmp(argv[1], "list") == 0) {
            // Print all tests: index name expected_code
            for (int i = 0; i < testCase_index; i++) {
                printf("%d %s %d\n", i, testCases[i].name, testCases[i].expectedReturn);
            }
            return 0;
        }
        else if (strcmp(argv[1], "runCase") == 0) {
            if (argc < 3) {
                fprintf(stderr, "Error: runCase requires index argument\n");
                return 1;
            }
            int index = atoi(argv[2]);
            if (index < 0 || index >= testCase_index) {
                fprintf(stderr, "Error: index %d out of range [0, %d)\n", index, testCase_index);
                return 1;
            }
            test_current_name = testCases[index].name;
            testCases[index].fp();
            return 0;  // Test passed (would have exited with 20 on failure)
        }
        else {
            fprintf(stderr, "Error: unknown command '%s'\n", argv[1]);
            print_usage(argv[0]);
            return 1;
        }
    }

    print_usage(argv[0]);
    return 0;
}
