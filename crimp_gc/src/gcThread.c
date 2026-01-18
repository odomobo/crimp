

#include "crimpGc.h"
#include "gcThread.h"


crimpGc_gcThread_t _crimpGc_gcThread;


void crimpGc_gcThread_state_init(crimpGc_gcThread_state_t* state)
{
    int e = pthread_mutex_init(&state->mutex, NULL);
    crimpGc_assert(e == 0);
    e = pthread_cond_init(&state->cv, NULL);
    crimpGc_assert(e == 0);
    state->state = GCTHREAD_NOT_COLLECTING;
}

void crimpGc_gcThread_data_init(crimpGc_gcThread_data_t* data)
{
    int e = pthread_mutex_init(&data->mutex, NULL);
    crimpGc_assert(e == 0);
    data->next_thread_id = 0;
    data->appThread_list = NULL;
}
