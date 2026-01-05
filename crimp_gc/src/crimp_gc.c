#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "crimp_gc.h"

static void* run(void* arguments)
{
    (void)arguments;
    printf("yep, this seemed to run on another thread!\n");
    sleep(1);
    printf("ok, worker thread exiting\n");
    return NULL;
}

void foo()
{
    printf("called foo\n");

    pthread_t thread;
    int result_code = pthread_create(&thread, NULL, run, NULL);
    assert(!result_code);
    pthread_join(thread, NULL);
    printf("joined!\n");
}