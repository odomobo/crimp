#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <threads.h>
#include "crimp_gc.h"

// TODO: use thread_local for stuff, like this:
// thread_local int tl = 0;

void crimp_gc_init() {
    log("[crimp_gc_init] entered\n");
    log("[crimp_gc_init] exited\n");
}

void crimp_gc_thread_register() {
    log("[crimp_gc_thread_register] entered\n");
    log("[crimp_gc_thread_register] exited\n");
}

void crimp_gc_thread_unregister() {
    log("[crimp_gc_thread_unregister] entered\n");
    log("[crimp_gc_thread_unregister] exited\n");
}