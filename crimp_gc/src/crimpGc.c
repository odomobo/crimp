#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <threads.h>
#include <stdatomic.h>

#include "crimpGc.h"
#include "shadowStack.h"
#include "appThreads.h"
#include "gcThread.h"
#include "logging.h"


void print_thread_list()
{
    printf("thread list: [");
    pthread_mutex_lock(&_crimpGc_gcThread.data.mutex);
    {
        crimpGc_appThread_t* t = _crimpGc_gcThread.data.appThread_list;
        while(t != NULL)
        {
            printf("%d, ", t->thread_id);
            t = t->next_thread;
        }
    }
    pthread_mutex_unlock(&_crimpGc_gcThread.data.mutex);
    printf("]\n");
}

static void* _crimpGc_collector(void* arguments) {
    (void)arguments;
    log("entered");

    // TODO: need to figure out if the process will be kept alive just because this thread is still executing; in fact, I don't think this thread can 
    while (true) {

        sleep(9999); // simulate waiting to be woken up
        // TODO: need to be able to be woken up
        // TODO: need to be able to read from threads list
        // TODO: need to be aware when a thread has died
    }

    // we can't actually reach here...
    return NULL;
}

void crimpGc_init() {
	crimpGc_logging_init();

    log("entered");

    ////////////////////////////////////////
    // init for gcThread

    crimpGc_gcThread_state_init(&_crimpGc_gcThread.state);
    crimpGc_gcThread_data_init(&_crimpGc_gcThread.data);

    // end init for gcThread
    ////////////////////////////////////////

    log("initialization completed; spawning collector thread");

    int result_code = pthread_create(&_crimpGc_gcThread.data.pthread, NULL, _crimpGc_collector, NULL);
    // TODO: better validation than simple crimpGc_assert... although we probably need to exit the application anyhow
    crimpGc_assert(!result_code);
    log("exited");
}

void crimpGc_thread_register() {
    log("entered");
    // TODO: move this logic into appThreads.c
    crimpGc_appThread = malloc(sizeof(*crimpGc_appThread));
    crimpGc_assert(crimpGc_appThread != NULL); // TODO: better check

    pthread_mutex_lock(&_crimpGc_gcThread.data.mutex);
    {
        crimpGc_appThread->thread_id = _crimpGc_gcThread.data.next_thread_id++;
        // add to threads list
        crimpGc_appThread->next_thread = _crimpGc_gcThread.data.appThread_list;
        _crimpGc_gcThread.data.appThread_list = crimpGc_appThread;

        crimpGc_appThread_state_init(&crimpGc_appThread->state);
        crimpGc_appThread_data_init(&crimpGc_appThread->data);
    }
    pthread_mutex_unlock(&_crimpGc_gcThread.data.mutex);
    log("exited");
}

void crimpGc_thread_unregister() {
    log("entered");
    
    pthread_mutex_lock(&_crimpGc_gcThread.data.mutex);
    {
        // remove from threads list
        crimpGc_appThread_t** t_ref = &_crimpGc_gcThread.data.appThread_list;
        while (true)
        {
            // we should find the current thread in the list somewhere...
            crimpGc_assert(*t_ref != NULL);
            // if we found the pointer to the current thread, update it to point to the next thread, and break
            if (*t_ref == crimpGc_appThread)
            {
                *t_ref = (*t_ref)->next_thread;
                break;
            }
            // otherwise, advance to the next thread pointer reference, and try again
            t_ref = &(*t_ref)->next_thread;
        }
    }
    pthread_mutex_unlock(&_crimpGc_gcThread.data.mutex);
    free(crimpGc_appThread);
    crimpGc_appThread = NULL;
    log("exited");
}