
#include "crimpGc.h"
#include "appThreads.h"

thread_local crimpGc_appThread_t* crimpGc_appThread = NULL;

void crimpGc_appThread_state_init(crimpGc_appThread_state_t* state)
{
    int e = pthread_mutex_init(&state->mutex, NULL);
    crimpGc_assert(e == 0);
    e = pthread_cond_init(&state->cv, NULL);
    crimpGc_assert(e == 0);
    state->state = APPTHREAD_NOT_COLLECTING;
    state->cleared_for_concurrent_collection = false;
}

void crimpGc_appThread_data_init(crimpGc_appThread_data_t* data)
{
    int e = pthread_mutex_init(&data->mutex, NULL);
    crimpGc_assert(e == 0);
    // TODO: init roots shadow stack
    // TODO: init handles roots
    // TODO: init gray lists
}
