#ifndef CRIMP_GC
#define CRIMP_GC

#include <stddef.h>
#include <stdbool.h>
#include <threads.h>

// Symbol visibility for shared library
#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef CRIMP_GC_BUILD
        #define CRIMP_GC_API __declspec(dllexport)
    #else
        #define CRIMP_GC_API __declspec(dllimport)
    #endif
#else
    #if __GNUC__ >= 4
        #define CRIMP_GC_API __attribute__((visibility("default")))
    #else
        #define CRIMP_GC_API
    #endif
#endif


/////////////////////////////////
// TYPES

// TODO: "mark function" type typedef

typedef struct crimp_type_t {
	// TODO: mark function
	const size_t size;
	
} crimp_type_t;

typedef struct crimpGc_slot_internal* crimpGc_slot;



// TYPES
/////////////////////////////////


/////////////////////////////////
// GC THREAD TYPES

enum crimpGc_gcThread_state_enum {
    COLLECTOR_NOT_COLLECTING,
    COLLECTOR_COLLECTING,
};

typedef struct crimpGc_gcThread_state_t {
    pthread_mutex_t mutex;
    enum crimpGc_gcThread_state_enum state;
} crimpGc_gcThread_state_t;


// GC THREAD TYPES
/////////////////////////////////


/////////////////////////////////
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
    pthread_mutex_t mutex;
    enum crimpGc_appThread_state_enum state;
    bool cleared_for_concurrent_collection;
} crimpGc_appThread_state_t;

struct crimpGc_appThread_t;

typedef struct crimpGc_appThread_t {
    int thread_id;
    struct crimpGc_appThread_t* next_thread;
    // TODO: roots shadow stack
    // TODO: thread local mutex
} crimpGc_appThread_t;

CRIMP_GC_API extern thread_local crimpGc_appThread_t* _crimpGc_appThread;

// APP THREADS TYPES
/////////////////////////////////


/////////////////////////////////
// TESTING

CRIMP_GC_API void print_thread_list();
CRIMP_GC_API extern bool _crimpGc_console_logging_enabled;
CRIMP_GC_API extern bool _crimpGc_file_logging_enabled;
CRIMP_GC_API FILE* _crimpGc_file_logging;

CRIMP_GC_API extern pthread_mutex_t _crimpGc_log_mutex;
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
/////////////////////////////////


/////////////////////////////////
// INIT

// TODO: global roots list

CRIMP_GC_API void crimpGc_init();
CRIMP_GC_API void crimpGc_global_roots_register();  // TODO: signature to support global roots
CRIMP_GC_API void crimpGc_thread_register();
CRIMP_GC_API void crimpGc_thread_unregister();

// INIT
/////////////////////////////////


/////////////////////////////////
// HANDLES

typedef struct crimpGc_handle_t {
    const void* data;  // Read-only access
    int _refcount;
} crimpGc_handle_t;

typedef struct crimpGc_builder_t {
    crimpGc_slot slot;
    int _refcount;
} crimpGc_builder_t;

CRIMP_GC_API crimpGc_handle_t* crimpGc_handle_acquire(crimpGc_slot* slot);
CRIMP_GC_API void crimpGc_handle_release(crimpGc_handle_t* handle);

CRIMP_GC_API crimpGc_builder_t* crimpGc_builder_create();
CRIMP_GC_API crimpGc_handle_t* crimpGc_builder_finish(crimpGc_builder_t* builder);  // Invalidates builder

CRIMP_GC_API void crimpGc_slot_set(crimpGc_slot* dst, const void* src);  // Write barrier

// HANDLES
/////////////////////////////////


/////////////////////////////////
// FRAME

// these should probably be macros
CRIMP_GC_API void crimpGc_frame_alloc();
CRIMP_GC_API crimpGc_slot* crimpGc_frame_push();
CRIMP_GC_API void crimpGc_frame_dealloc(int n);

// FRAME
/////////////////////////////////


/////////////////////////////////
// COLLECT/MALLOC

CRIMP_GC_API void crimpGc_collect();
CRIMP_GC_API void crimpGc_malloc(crimp_type_t* type);
CRIMP_GC_API void crimpGc_malloc_array(crimp_type_t* type, int count);

// COLLECT/MALLOC
/////////////////////////////////


#endif // CRIMP_GC