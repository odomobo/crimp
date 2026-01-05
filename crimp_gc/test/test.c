#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <threads.h>
#include "crimp_gc.h"

thread_local int tl = 0;

static void* run(void* arguments)
{
    (void)arguments;
    crimp_gc_thread_register();
    log("[run] yep, this seemed to run on another thread!\n");
    log("[run] tl is %d; setting to 3\n", tl);
    tl = 3;
    log("[run] pausing...");
    fflush(stdout);
    usleep(200 * 1000); // 200 ms
    log(" done!\n");
    log("[run] tl is still %d\n", tl);
    log("[run] ok, worker thread exiting\n");
    crimp_gc_thread_unregister();
    return NULL;
}

void foo()
{
    tl = 1;
    log("[foo] set tl to 1\n");

    pthread_t thread;
    int result_code = pthread_create(&thread, NULL, run, NULL);
    assert(!result_code);
    pthread_join(thread, NULL);
    log("[foo] joined!\n");
    log("[foo] tl is still %d\n", tl);
}

int main(void) {
    crimp_gc_init();
    crimp_gc_thread_register();
    foo();
    crimp_gc_thread_unregister();
    return 0;
}
