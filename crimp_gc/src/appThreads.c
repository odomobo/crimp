#include <stdlib.h>

#include "crimpGc_internals.h"

thread_local crimpGc_appThread_t* crimpGc_appThread = NULL;

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

void crimpGc_appThread_register() {
    log("entered");
    // TODO: move this logic into appThreads.c
    crimpGc_appThread = malloc(sizeof(*crimpGc_appThread));
    crimpGc_assert(crimpGc_appThread != NULL); // TODO: better check

    _crimpGc_appThread_state_init(&crimpGc_appThread->state);
    _crimpGc_appThread_data_init(&crimpGc_appThread->data);

    pthread_mutex_lock(&_crimpGc_gcThread.data.mutex);
    {
        crimpGc_appThread->thread_id = _crimpGc_gcThread.data.next_thread_id++;
        // add to threads list
        crimpGc_appThread->next_thread = _crimpGc_gcThread.data.appThread_list;
        _crimpGc_gcThread.data.appThread_list = crimpGc_appThread;    
    }
    pthread_mutex_unlock(&_crimpGc_gcThread.data.mutex);
    log("exited");
}

void crimpGc_appThread_unregister() {
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