#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <threads.h>
#include <stdatomic.h>
#include "crimp_gc.h"

// TODO: use thread_local for stuff, like this:
// thread_local int tl = 0;

pthread_mutex_t _crimp_gc_log_mutex = PTHREAD_MUTEX_INITIALIZER;

thread_local crimp_gc_thread_t* _crimp_gc_thread = NULL;
pthread_mutex_t _crimp_gc_thread_list_mutex = PTHREAD_MUTEX_INITIALIZER;
// changes to this need to be locked with thread_list_lock
int _crimp_gc_next_thread_id = 0;
crimp_gc_thread_t* _crimp_gc_thread_list = NULL;

pthread_t _crimp_gc_collector_pthread;

void print_thread_list()
{
    printf("thread list: [");
    pthread_mutex_lock(&_crimp_gc_thread_list_mutex);
    {
        crimp_gc_thread_t* t = _crimp_gc_thread_list;
        while(t != NULL)
        {
            printf("%d, ", t->thread_id);
            t = t->next_thread;
        }
    }
    pthread_mutex_unlock(&_crimp_gc_thread_list_mutex);
    printf("]\n");
}

static void* _crimp_gc_collector(void* arguments) {
    (void)arguments;
    log("entered");
    
    sleep(9999); // simulate doing nothing
    // TODO: need to be able to be woken up
    // TODO: need to be able to read from threads list
    // TODO: need to be aware when a thread has died
    log("exited");
    return NULL;
}

void crimp_gc_init() {
    log("entered");

    /////////////////////////////////
    // init for gc_collector thread

    // we only do this for logging purposes
    // TODO: we should probably remove this eventually, if we don't need it for logging
    _crimp_gc_thread = malloc(sizeof(*_crimp_gc_thread));
    _crimp_gc_thread->thread_id = -1;

    // end init for gc_collector thread
    /////////////////////////////////

    log("initialization completed; spawning collector thread");

    int result_code = pthread_create(&_crimp_gc_collector_pthread, NULL, _crimp_gc_collector, NULL);
    // TODO: better validation than simple assert... although we probably need to exit the application anyhow
    assert(!result_code);
    log("exited");
}

void crimp_gc_thread_register() {
    log("entered");
    _crimp_gc_thread = malloc(sizeof(*_crimp_gc_thread));

    pthread_mutex_lock(&_crimp_gc_thread_list_mutex);
    {
        _crimp_gc_thread->thread_id = _crimp_gc_next_thread_id++;
        // add to threads list
        _crimp_gc_thread->next_thread = _crimp_gc_thread_list;
        _crimp_gc_thread_list = _crimp_gc_thread;
    }
    pthread_mutex_unlock(&_crimp_gc_thread_list_mutex);
    log("exited");
}

void crimp_gc_thread_unregister() {
    log("entered");
    
    pthread_mutex_lock(&_crimp_gc_thread_list_mutex);
    {
        // remove from threads list
        crimp_gc_thread_t** t_ref = &_crimp_gc_thread_list;
        while (true)
        {
            // we should find the current thread in the list somewhere...
            assert(*t_ref != NULL);
            // if we found the pointer to the current thread, update it to point to the next thread, and break
            if (*t_ref == _crimp_gc_thread)
            {
                *t_ref = (*t_ref)->next_thread;
                break;
            }
            // otherwise, advance to the next thread pointer reference, and try again
            t_ref = &(*t_ref)->next_thread;
        }
    }
    pthread_mutex_unlock(&_crimp_gc_thread_list_mutex);
    free(_crimp_gc_thread);
    _crimp_gc_thread = NULL;
    log("exited");
}