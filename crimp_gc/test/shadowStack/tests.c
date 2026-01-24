#include "../test.h"

TESTCASE(shadowStack_test1, SUCCESS) {
    test_assert(1 == 1);
}

TESTCASE(shadowStack_test2, ASSERT_FAIL) {
    test_assert(1 == 2);
}

TESTCASE(shadowStack_popNull_yieldsError, ASSERT_FAIL) {
    crimpGc_shadowStack_frame_pop(NULL);
}