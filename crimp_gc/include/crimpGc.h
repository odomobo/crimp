#ifndef CRIMPGC
#define CRIMPGC

#include <stddef.h>
#include <stdbool.h>
#include <threads.h>
#include <stdlib.h>
#include <stdio.h>

// Symbol visibility for shared library
#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef CRIMPGC_BUILD
        #define CRIMPGC_API __declspec(dllexport)
    #else
        #define CRIMPGC_API __declspec(dllimport)
    #endif
#else
    #if __GNUC__ >= 4
        #define CRIMPGC_API __attribute__((visibility("default")))
    #else
        #define CRIMPGC_API
    #endif
#endif


#define crimpGc_assert(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "Assertion failed: ( %s ) in %s on line #%d\n", #condition, __FILE__, __LINE__); \
        exit(1); \
    } \
} while (0)


////////////////////////////////////////
// TYPES

// TODO: "mark function" type typedef

typedef struct crimp_type_t {
	// TODO: mark function
	const size_t size;
	
} crimp_type_t;

typedef struct crimpGc_slot_internal_t* crimpGc_slot_t;

typedef struct crimpGc_grayList_t crimpGc_grayList_t;

// TYPES
////////////////////////////////////////


////////////////////////////////////////
// ACQUIRING MUTEXES
//
// When acquiring mutexes, if holding multiple ones, they must be done with this partial ordering:
// 1. appThread->state.mutex
// 2. appThread->data.mutex
// 3. gcThread.state.mutex
// 4. gcThread.data.mutex
//
// That's not to say that you have to hold all of them, but that if you have a lock on gcThread.state.mutex, you cannot hold appThread->state.mutex without first releasing gcThread.state.mutex. You are then free to reacquire gcThread.state.mutex afterwards.
//
// By enforcing this ordering, it prevents deadlocks.
//
// ACQUIRING MUTEXES
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

struct crimpGc_appThread_t;

// Note: need to hold _crimpGc_gcThread.data.mutex to use appThread itself (but not for state or data)
typedef struct crimpGc_appThread_t {
    int thread_id;
    struct crimpGc_appThread_t* next_thread;

    crimpGc_appThread_state_t state;
    crimpGc_appThread_data_t data;
} crimpGc_appThread_t;

CRIMPGC_API extern thread_local crimpGc_appThread_t* _crimpGc_appThread;

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
    crimpGc_appThread_t* appThread_list;
    pthread_t pthread;
    // TODO: stuff????
} crimpGc_gcThread_data_t;

typedef struct crimpGc_gcThread_t {
    crimpGc_gcThread_state_t state;
    crimpGc_gcThread_data_t data;
} crimpGc_gcThread_t;

CRIMPGC_API extern crimpGc_gcThread_t _crimpGc_gcThread;

// GC THREAD TYPES
////////////////////////////////////////


////////////////////////////////////////
// TESTING

CRIMPGC_API void print_thread_list();
CRIMPGC_API extern bool _crimpGc_console_logging_enabled;
CRIMPGC_API extern bool _crimpGc_file_logging_enabled;
CRIMPGC_API extern FILE* _crimpGc_file_logging;

CRIMPGC_API extern pthread_mutex_t _crimpGc_log_mutex;
#define log(...) do{  \
	pthread_mutex_lock(&_crimpGc_log_mutex);  \
	if (_crimpGc_console_logging_enabled) {  \
		_crimpGc_appThread != NULL ? fprintf(stderr, " (%02d) [%s] ", _crimpGc_appThread->thread_id, __func__) : fprintf(stderr, " (\?\?) [%s] ", __func__);  \
		fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n");  \
	}  \
	if (_crimpGc_file_logging != NULL) {  \
		_crimpGc_appThread != NULL ? fprintf(_crimpGc_file_logging, " (%02d) [%s] ", _crimpGc_appThread->thread_id, __func__) : fprintf(_crimpGc_file_logging, " (\?\?) [%s] ", __func__);  \
		fprintf(_crimpGc_file_logging, __VA_ARGS__); fprintf(_crimpGc_file_logging, "\n");  \
		fflush(_crimpGc_file_logging);  \
	}  \
	pthread_mutex_unlock(&_crimpGc_log_mutex);  \
}while(false)
// #define log(...) 

// TESTING
////////////////////////////////////////


////////////////////////////////////////
// INIT

// TODO: global roots list

CRIMPGC_API void crimpGc_init();
CRIMPGC_API void crimpGc_global_roots_register();  // TODO: signature to support global roots
CRIMPGC_API void crimpGc_thread_register();
CRIMPGC_API void crimpGc_thread_unregister();

// INIT
////////////////////////////////////////


////////////////////////////////////////
// HANDLES

typedef struct crimpGc_handle_t {
    const void* data;  // Read-only access
    int _refcount;
} crimpGc_handle_t;

typedef struct crimpGc_builder_t {
    crimpGc_slot_t slot;
    int _refcount;
} crimpGc_builder_t;

CRIMPGC_API crimpGc_handle_t* crimpGc_handle_acquire(crimpGc_slot_t* slot);
CRIMPGC_API void crimpGc_handle_release(crimpGc_handle_t* handle);

CRIMPGC_API crimpGc_builder_t* crimpGc_builder_create();
CRIMPGC_API crimpGc_handle_t* crimpGc_builder_finish(crimpGc_builder_t* builder);  // Invalidates builder

CRIMPGC_API void crimpGc_slot_set(crimpGc_slot_t* dst, const void* src);  // Write barrier

// HANDLES
////////////////////////////////////////


////////////////////////////////////////
// FRAME

// these should probably be macros
CRIMPGC_API void crimpGc_frame_alloc();
CRIMPGC_API crimpGc_slot_t* crimpGc_frame_push();
CRIMPGC_API void crimpGc_frame_dealloc(int n);

// FRAME
////////////////////////////////////////


////////////////////////////////////////
// COLLECT/MALLOC

CRIMPGC_API void crimpGc_collect();
CRIMPGC_API void crimpGc_malloc(crimp_type_t* type);
CRIMPGC_API void crimpGc_malloc_array(crimp_type_t* type, int count);

// COLLECT/MALLOC
////////////////////////////////////////


#endif // CRIMPGC