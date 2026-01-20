#ifndef CRIMPGC_GCTHREAD
#define CRIMPGC_GCTHREAD

#include <pthread.h>


////////////////////////////////////////
// GC THREAD TYPES

enum crimpGc_gcThread_state_enum {
    GCTHREAD_NOT_COLLECTING,
    GCTHREAD_COLLECTING,
};

typedef struct crimpGc_gcThread_state_t {
    pthread_mutex_t mutex; // 3
    pthread_cond_t cv;
    enum crimpGc_gcThread_state_enum state;
} crimpGc_gcThread_state_t;

typedef struct crimpGc_gcThread_data_t {
    pthread_mutex_t mutex; // 4
    int next_thread_id;
    struct crimpGc_appThread_t* appThread_list;
    pthread_t pthread;
    // TODO: stuff????
} crimpGc_gcThread_data_t;

typedef struct crimpGc_gcThread_t {
    crimpGc_gcThread_state_t state;
    crimpGc_gcThread_data_t data;
} crimpGc_gcThread_t;

extern crimpGc_gcThread_t _crimpGc_gcThread;

// GC THREAD TYPES
////////////////////////////////////////


void crimpGc_gcThread_init();
//CRIMPGC_API void crimpGc_gcThread_beginCollecting();


#endif // CRIMPGC_GCTHREAD