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


// TODO: move to debug?
CRIMPGC_API void print_thread_list()
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

CRIMPGC_API void crimpGc_init() {
	crimpGc_logging_init();
    crimpGc_gcThread_init();
}

// wrapper function
CRIMPGC_API void crimpGc_thread_register() {
    crimpGc_appThread_register();
}

// wrapper function
CRIMPGC_API void crimpGc_thread_unregister() {
    crimpGc_appThread_unregister();
}