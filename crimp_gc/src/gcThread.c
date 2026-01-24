#include <unistd.h>
#include <stdbool.h>

#include "crimpGc_internals.h"


crimpGc_gcThread_t _crimpGc_gcThread;


static void* _crimpGc_gcThread_run(void* arguments) {
    (void)arguments;
    int e;
    log("entered");

    // TODO: need to figure out if the process will be kept alive just because this thread is still executing; in fact, I don't think this thread can 

    while (true) {
        // wait until we're put into the "collecting" state by another thread
        e = pthread_mutex_lock(&_crimpGc_gcThread.state.mutex); crimpGc_assert(!e);
        while (true) {
            e = pthread_cond_wait(&_crimpGc_gcThread.state.cv, &_crimpGc_gcThread.state.mutex); crimpGc_assert(e == 0);
            if (_crimpGc_gcThread.state.state == GCTHREAD_COLLECTING) {
                break;
            }
        }
        e = pthread_mutex_unlock(&_crimpGc_gcThread.state.mutex); crimpGc_assert(!e);
        log("saw state GCTHREAD_COLLECTING; begin collecting");

        // TODO: collection logic here
        log("should collect... but instead, we're doing nothing!");

        log("finished collecting");
        e = pthread_mutex_lock(&_crimpGc_gcThread.state.mutex); crimpGc_assert(!e);
        {
            log("transition to state GCTHREAD_NOT_COLLECTING");
            _crimpGc_gcThread.state.state = GCTHREAD_NOT_COLLECTING;
        }
        e = pthread_mutex_unlock(&_crimpGc_gcThread.state.mutex); crimpGc_assert(!e);
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

void crimpGc_gcThread_init() {
    log("entered");

    ////////////////////////////////////////
    // init for gcThread

    _crimpGc_gcThread_state_init(&_crimpGc_gcThread.state);
    _crimpGc_gcThread_data_init(&_crimpGc_gcThread.data);

    // end init for gcThread
    ////////////////////////////////////////

    log("initialization completed; spawning collector thread");

    int result_code = pthread_create(&_crimpGc_gcThread.data.pthread, NULL, _crimpGc_gcThread_run, NULL);
    // TODO: better validation than simple crimpGc_assert... although we probably need to exit the application anyhow
    crimpGc_assert(!result_code);
    log("exited");
}

CRIMPGC_API void crimpGc_gcThread_beginCollecting() {
    int e = pthread_mutex_lock(&_crimpGc_gcThread.state.mutex);
    crimpGc_assert(!e);
    {
        log("transition to state GCTHREAD_COLLECTING");
        _crimpGc_gcThread.state.state = GCTHREAD_COLLECTING;
        e = pthread_cond_signal(&_crimpGc_gcThread.state.cv);
        crimpGc_assert(!e);
    }
    e = pthread_mutex_unlock(&_crimpGc_gcThread.state.mutex);
    crimpGc_assert(!e);
}