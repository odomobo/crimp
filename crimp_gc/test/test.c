#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <threads.h>

#include "crimpGc.h"

thread_local int tl = 0;

static void* run(void* arguments)
{
    (void)arguments;
    crimpGc_thread_register();
    log("yep, this seemed to run on another thread!");
    log("tl is %d; setting to 3", tl);
    tl = 3;
    log("triggering a collection");
    crimpGc_gcThread_beginCollecting();
    log("pausing...");
    fflush(stdout);
    usleep(200 * 1000); // 200 ms
    log("done!");
    print_thread_list();
    log("tl is still %d", tl);
    log("ok, worker thread exiting");
    crimpGc_thread_unregister();
    return NULL;
}

void foo()
{
    tl = 1;
    log("set tl to 1");

    pthread_t thread;
    int result_code = pthread_create(&thread, NULL, run, NULL);
    crimpGc_assert(!result_code);
    pthread_join(thread, NULL);
    log("joined!");
    log("tl is still %d", tl);
}

int main(void) {
    crimpGc_init();
    crimpGc_thread_register();
    print_thread_list();
    foo();
    print_thread_list();
    crimpGc_thread_unregister();
    return 0;
}
