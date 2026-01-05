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
    log("yep, this seemed to run on another thread!");
    log("tl is %d; setting to 3", tl);
    tl = 3;
    log("pausing...");
    fflush(stdout);
    usleep(200 * 1000); // 200 ms
    log("done!");
    print_thread_list();
    log("tl is still %d", tl);
    log("ok, worker thread exiting");
    crimp_gc_thread_unregister();
    return NULL;
}

void foo()
{
    tl = 1;
    log("set tl to 1");

    pthread_t thread;
    int result_code = pthread_create(&thread, NULL, run, NULL);
    assert(!result_code);
    pthread_join(thread, NULL);
    log("joined!");
    log("tl is still %d", tl);
}

int main(void) {
    crimp_gc_init();
    crimp_gc_thread_register();
    print_thread_list();
    foo();
    print_thread_list();
    crimp_gc_thread_unregister();
    return 0;
}
