#ifndef CRIMPGC_APPTHREADS
#define CRIMPGC_APPTHREADS

#include <pthread.h>


////////////////////////////////////////
// APP THREADS TYPES

enum crimpGc_appThread_state_enum {
    APPTHREAD_NOT_COLLECTING,
    APPTHREAD_MARK_ROOTS,
    APPTHREAD_APPTHREAD_BLOCKED_MARKING_ROOTS,
    APPTHREAD_AWAITING_CONCURRENT_COLLECTION,
    APPTHREAD_COLLECTOR_BLOCKED_MARKING_ROOTS,
    APPTHREAD_CONCURRENT_COLLECTION,
    APPTHREAD_COLLECTOR_BLOCKED_FLUSHING_GRAY_LISTS,
};

typedef struct crimpGc_appThread_state_t {
    pthread_mutex_t mutex; // 1
    pthread_cond_t cv;
    enum crimpGc_appThread_state_enum state;
    bool cleared_for_concurrent_collection;
} crimpGc_appThread_state_t;

typedef struct crimpGc_appThread_data_t {
    pthread_mutex_t mutex; // 2
    // TODO: roots shadow stack
    // TODO: roots handles
    // TODO: gray list
} crimpGc_appThread_data_t;

struct crimpGc_appThread_t;

// Note: need to hold _crimpGc_gcThread.data.mutex to use appThread itself (but not for state or data)
typedef struct crimpGc_appThread_t {
    int thread_id;
    struct crimpGc_appThread_t* next_thread;

    crimpGc_appThread_state_t state;
    crimpGc_appThread_data_t data;
} crimpGc_appThread_t;

extern thread_local crimpGc_appThread_t* crimpGc_appThread;

// APP THREADS TYPES
////////////////////////////////////////


void crimpGc_appThread_state_init(crimpGc_appThread_state_t* state);
void crimpGc_appThread_data_init(crimpGc_appThread_data_t* data);

#endif // CRIMPGC_APPTHREADS