//#include <assert.h>
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
crimpGc_gcThread_t _crimpGc_gcThread;

pthread_t _crimpGc_collector_pthread;

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

static void _crimpGc_gcThread_state_init(crimpGc_gcThread_state_t* state)
{
    int e = pthread_mutex_init(&state->mutex, NULL);
    crimpGc_assert(e == 0);
    e = pthread_cond_init(&state->cv, NULL);
    crimpGc_assert(e == 0);
    state->state = GCTHREAD_NOT_COLLECTING;
}

static void _crimpGc_gcThread_data_init(crimpGc_gcThread_data_t* data)
{
    int e = pthread_mutex_init(&data->mutex, NULL);
    crimpGc_assert(e == 0);
    data->next_thread_id = 0;
    data->appThread_list = NULL;
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

    _crimpGc_gcThread_state_init(&_crimpGc_gcThread.state);
    _crimpGc_gcThread_data_init(&_crimpGc_gcThread.data);

    // end init for gc_collector thread
    /////////////////////////////////

    log("initialization completed; spawning collector thread");

    int result_code = pthread_create(&_crimpGc_collector_pthread, NULL, _crimpGc_collector, NULL);
    // TODO: better validation than simple crimpGc_assert... although we probably need to exit the application anyhow
    crimpGc_assert(!result_code);
    log("exited");
}

static void _crimpGc_appThread_state_init(crimpGc_appThread_state_t* state)
{
    int e = pthread_mutex_init(&state->mutex, NULL);
    crimpGc_assert(e == 0);
    e = pthread_cond_init(&state->cv, NULL);
    crimpGc_assert(e == 0);
    state->state = APPTHREAD_NOT_COLLECTING;
    state->cleared_for_concurrent_collection = false;
}

static void _crimpGc_appThread_data_init(crimpGc_appThread_data_t* data)
{
    int e = pthread_mutex_init(&data->mutex, NULL);
    crimpGc_assert(e == 0);
    // TODO: init roots shadow stack
    // TODO: init handles roots
    // TODO: init gray lists
}

void crimpGc_thread_register() {
    log("entered");
    _crimpGc_appThread = malloc(sizeof(*_crimpGc_appThread));
    crimpGc_assert(_crimpGc_appThread != NULL); // TODO: better check

    pthread_mutex_lock(&_crimpGc_gcThread.data.mutex);
    {
        _crimpGc_appThread->thread_id = _crimpGc_gcThread.data.next_thread_id++;
        // add to threads list
        _crimpGc_appThread->next_thread = _crimpGc_gcThread.data.appThread_list;
        _crimpGc_gcThread.data.appThread_list = _crimpGc_appThread;

        _crimpGc_appThread_state_init(&_crimpGc_appThread->state);
        _crimpGc_appThread_data_init(&_crimpGc_appThread->data);
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
            if (*t_ref == _crimpGc_appThread)
            {
                *t_ref = (*t_ref)->next_thread;
                break;
            }
            // otherwise, advance to the next thread pointer reference, and try again
            t_ref = &(*t_ref)->next_thread;
        }
    }
    pthread_mutex_unlock(&_crimpGc_gcThread.data.mutex);
    free(_crimpGc_appThread);
    _crimpGc_appThread = NULL;
    log("exited");
}