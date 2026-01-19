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
typedef void (crimpGc_mark_fp) (void* object);

typedef struct crimp_type_t {
	crimpGc_mark_fp* mark_fn;
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
// TESTING

CRIMPGC_API void print_thread_list();

// TESTING
////////////////////////////////////////


////////////////////////////////////////
// LOGGING

CRIMPGC_API void crimpGc_log(char const* func, char const* fmt, ...);
#define log(fmt, ...) crimpGc_log(__func__, fmt, ##__VA_ARGS__)
// #define log(fmt, ...) 

// LOGGING
////////////////////////////////////////



////////////////////////////////////////
// INIT

// TODO: global roots list

CRIMPGC_API void crimpGc_init();
CRIMPGC_API void crimpGc_gcThread_beginCollecting();
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