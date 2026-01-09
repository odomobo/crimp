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

struct crimp_gc_thread_t;

typedef struct crimp_gc_thread_t {
    int thread_id;
    struct crimp_gc_thread_t* next_thread;
    // TODO: roots shadow stack
    // TODO: thread local mutex
} crimp_gc_thread_t;

// TODO: "mark function" type typedef

typedef struct crimp_type_t {
	// TODO: mark function
	const size_t size;
	
} crimp_type_t;

typedef struct crimp_gc_slot_internal* crimp_gc_slot;

CRIMP_GC_API extern thread_local crimp_gc_thread_t* _crimp_gc_thread;

// TYPES
/////////////////////////////////

/////////////////////////////////
// TESTING

CRIMP_GC_API void print_thread_list();
CRIMP_GC_API extern bool _crimp_gc_console_logging_enabled;
CRIMP_GC_API extern bool _crimp_gc_file_logging_enabled;
CRIMP_GC_API FILE* _crimp_gc_file_logging;

CRIMP_GC_API extern pthread_mutex_t _crimp_gc_log_mutex;
#define log(...) do{  \
	pthread_mutex_lock(&_crimp_gc_log_mutex);  \
	if (_crimp_gc_console_logging_enabled) {  \
		_crimp_gc_thread != NULL ? fprintf(stderr, " (%02d) [%s] ", _crimp_gc_thread->thread_id, __func__) : fprintf(stderr, " (\?\?) [%s] ", __func__);  \
		fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n");  \
	}  \
	if (_crimp_gc_file_logging != NULL) {  \
		_crimp_gc_thread != NULL ? fprintf(_crimp_gc_file_logging, " (%02d) [%s] ", _crimp_gc_thread->thread_id, __func__) : fprintf(_crimp_gc_file_logging, " (\?\?) [%s] ", __func__);  \
		fprintf(_crimp_gc_file_logging, __VA_ARGS__); fprintf(_crimp_gc_file_logging, "\n");  \
		fflush(_crimp_gc_file_logging);  \
	}  \
	pthread_mutex_unlock(&_crimp_gc_log_mutex);  \
}while(false)
// #define log(...) 

// TESTING
/////////////////////////////////

/////////////////////////////////
// INIT

// TODO: global roots list

CRIMP_GC_API void crimp_gc_init();
CRIMP_GC_API void crimp_gc_global_roots_register();  // TODO: signature to support global roots
CRIMP_GC_API void crimp_gc_thread_register();
CRIMP_GC_API void crimp_gc_thread_unregister();

// INIT
/////////////////////////////////

/////////////////////////////////
// HANDLES

typedef struct crimp_gc_handle_t {
    const void* data;  // Read-only access
    int _refcount;
} crimp_gc_handle_t;

typedef struct crimp_gc_builder_t {
    crimp_gc_slot slot;
    int _refcount;
} crimp_gc_builder_t;

CRIMP_GC_API crimp_gc_handle_t* crimp_gc_handle_acquire(crimp_gc_slot* slot);
CRIMP_GC_API void crimp_gc_handle_release(crimp_gc_handle_t* handle);

CRIMP_GC_API crimp_gc_builder_t* crimp_gc_builder_create();
CRIMP_GC_API crimp_gc_handle_t* crimp_gc_builder_finish(crimp_gc_builder_t* builder);  // Invalidates builder

CRIMP_GC_API void crimp_gc_slot_set(crimp_gc_slot* dst, const void* src);  // Write barrier

// HANDLES
/////////////////////////////////

/////////////////////////////////
// FRAME

// these should probably be macros
CRIMP_GC_API void crimp_gc_frame_alloc();
CRIMP_GC_API crimp_gc_slot* crimp_gc_frame_push();
CRIMP_GC_API void crimp_gc_frame_dealloc(int n);

// FRAME
/////////////////////////////////

/////////////////////////////////
// COLLECT/MALLOC

CRIMP_GC_API void crimp_gc_collect();
CRIMP_GC_API void crimp_gc_malloc(crimp_type_t* type);
CRIMP_GC_API void crimp_gc_malloc_array(crimp_type_t* type, int count);

// COLLECT/MALLOC
/////////////////////////////////

#endif // CRIMP_GC