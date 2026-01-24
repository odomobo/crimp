#ifndef CRIMPGC_INTERNALS
#define CRIMPGC_INTERNALS

// INTERNAL HEADER - FOR TESTING AND IMPLEMENTATION ONLY
// Production code should only include "crimpGc.h"
//
// This header exposes internal implementation details for testing purposes.
// Functions and types declared here are subject to change without notice.

#include <pthread.h>
#include <stdbool.h>
#include <threads.h>

#include "crimpGc.h"


////////////////////////////////////////
// INTERNAL TYPES

enum crimpGc_color {
    WHITE,
    GRAY,
    BLACK,
};

typedef struct crimpGc_typeHeader_t {
    crimp_type_t* type;
    enum crimpGc_color color;
} crimpGc_typeHeader_t;

// we want to obscure slot types, to prevent their misuse
typedef struct crimpGc_slot_internal_t {
    crimpGc_typeHeader_t typeHeader;
} crimpGc_slot_internal_t;

// INTERNAL TYPES
////////////////////////////////////////


////////////////////////////////////////
// LOGGING

// defined in crimpGc.h
// CRIMPGC_API void crimpGc_log(char* func, char* fmt, ...);
CRIMPGC_API void crimpGc_logging_init();

// LOGGING
////////////////////////////////////////


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

// Note: need to hold _crimpGc_gcThread.data.mutex to use appThread itself (but not for state or data)
typedef struct crimpGc_appThread_t {
    int thread_id;
    struct crimpGc_appThread_t* next_thread;

    crimpGc_appThread_state_t state;
    crimpGc_appThread_data_t data;
} crimpGc_appThread_t;

extern thread_local crimpGc_appThread_t* crimpGc_appThread;

CRIMPGC_API void crimpGc_appThread_register();
CRIMPGC_API void crimpGc_appThread_unregister();

// APP THREADS TYPES
////////////////////////////////////////


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

CRIMPGC_API void crimpGc_gcThread_init();
//CRIMPGC_API void crimpGc_gcThread_beginCollecting();

// GC THREAD TYPES
////////////////////////////////////////


////////////////////////////////////////
// SHADOW STACK

typedef struct crimpGc_shadowStack_frame_t {
    int roots_count;
    crimpGc_slot_t* roots;
    struct crimpGc_shadowStack_frame_t* prev_frame;
} crimpGc_shadowStack_frame_t;

CRIMPGC_API crimpGc_shadowStack_frame_t* crimpGc_shadowStack_frame_push(int roots_count);
CRIMPGC_API void crimpGc_shadowStack_frame_pop(crimpGc_shadowStack_frame_t* frame);
CRIMPGC_API crimpGc_slot_t* crimpGc_shadowStack_frame_getSlot(crimpGc_shadowStack_frame_t* frame, int slot_index);
CRIMPGC_API void crimpGc_shadowStack_markRoots(crimpGc_grayList_t* grayList);

// SHADOW STACK
////////////////////////////////////////


////////////////////////////////////////
// GRAY LIST

CRIMPGC_API void crimpGc_grayList_push(crimpGc_grayList_t* grayList, crimpGc_typeHeader_t* obj);
CRIMPGC_API crimpGc_typeHeader_t* crimpGc_grayList_pop(crimpGc_grayList_t* grayList);

// GRAY LIST
////////////////////////////////////////


////////////////////////////////////////
// MARK

CRIMPGC_API void crimpGc_mark_gray(crimpGc_grayList_t* grayList, crimpGc_typeHeader_t* obj);
CRIMPGC_API void crimpGc_mark_black(crimpGc_grayList_t* grayList, crimpGc_typeHeader_t* obj);

// MARK
////////////////////////////////////////


////////////////////////////////////////
// STACK ALLOCATOR



// STACK ALLOCATOR
////////////////////////////////////////


#endif // CRIMPGC_INTERNALS
