#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <threads.h>
#include <stdatomic.h>
#include "crimp_gc.h"

////////////////////////////////////////////////////////////
// Logging

pthread_mutex_t _crimpGc_log_mutex = PTHREAD_MUTEX_INITIALIZER;
bool _crimpGc_console_logging_enabled = true;
bool _crimpGc_file_logging_enabled = false;
FILE* _crimpGc_file_logging = NULL;

// Logging
////////////////////////////////////////////////////////////


thread_local crimpGc_appThread_t* _crimpGc_appThread = NULL;

// this mutex is used for every global gc lock, which should be very rare anyhow
pthread_mutex_t _crimpGc_gc_mutex = PTHREAD_MUTEX_INITIALIZER;

////////////////////////////////////////////////////////////
// These need to be locked with _crimpGc_gc_mutex to be used

int _crimpGc_next_thread_id = 0;
crimpGc_appThread_t* _crimpGc_appThread_list = NULL;

// End locked globals
////////////////////////////////////////////////////////////

pthread_t _crimpGc_collector_pthread;

void print_thread_list()
{
    printf("thread list: [");
    pthread_mutex_lock(&_crimpGc_gc_mutex);
    {
        crimpGc_appThread_t* t = _crimpGc_appThread_list;
        while(t != NULL)
        {
            printf("%d, ", t->thread_id);
            t = t->next_thread;
        }
    }
    pthread_mutex_unlock(&_crimpGc_gc_mutex);
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
	/////////////////////////////////
	// LOGGING
	if (_crimpGc_file_logging_enabled) {
		char filename[1024];
		sprintf(filename, "crimpGc_%lu.log", (unsigned long)time(NULL));
		_crimpGc_file_logging = fopen(filename, "w");
	}
	// LOGGING
	/////////////////////////////////

    log("entered");

    /////////////////////////////////
    // init for gc_collector thread

    // we only do this for logging purposes
    // TODO: we should probably remove this eventually, if we don't need it for logging
    _crimpGc_appThread = malloc(sizeof(*_crimpGc_appThread));
    assert(_crimpGc_appThread != NULL); // TODO: better check
    _crimpGc_appThread->thread_id = -1;

    // end init for gc_collector thread
    /////////////////////////////////

    log("initialization completed; spawning collector thread");

    int result_code = pthread_create(&_crimpGc_collector_pthread, NULL, _crimpGc_collector, NULL);
    // TODO: better validation than simple assert... although we probably need to exit the application anyhow
    assert(!result_code);
    log("exited");
}

void crimpGc_thread_register() {
    log("entered");
    _crimpGc_appThread = malloc(sizeof(*_crimpGc_appThread));
    assert(_crimpGc_appThread != NULL); // TODO: better check

    pthread_mutex_lock(&_crimpGc_gc_mutex);
    {
        _crimpGc_appThread->thread_id = _crimpGc_next_thread_id++;
        // add to threads list
        _crimpGc_appThread->next_thread = _crimpGc_appThread_list;
        _crimpGc_appThread_list = _crimpGc_appThread;
    }
    pthread_mutex_unlock(&_crimpGc_gc_mutex);
    log("exited");
}

void crimpGc_thread_unregister() {
    log("entered");
    
    pthread_mutex_lock(&_crimpGc_gc_mutex);
    {
        // remove from threads list
        crimpGc_appThread_t** t_ref = &_crimpGc_appThread_list;
        while (true)
        {
            // we should find the current thread in the list somewhere...
            assert(*t_ref != NULL);
            // if we found the pointer to the current thread, update it to point to the next thread, and break
            if (*t_ref == _crimpGc_appThread)
            {
                *t_ref = (*t_ref)->next_thread;
                break;
            }
            // otherwise, advance to the next thread pointer reference, and try again
            t_ref = &(*t_ref)->next_thread;
        }
    }
    pthread_mutex_unlock(&_crimpGc_gc_mutex);
    free(_crimpGc_appThread);
    _crimpGc_appThread = NULL;
    log("exited");
}