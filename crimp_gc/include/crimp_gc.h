#ifndef CRIMP_GC
#define CRIMP_GC

#include <stddef.h>
#include <stdbool.h>
#include <threads.h>

/////////////////////////////////
// TYPES

struct crimp_gc_thread_t;

typedef struct crimp_gc_thread_t {
    int thread_id;
    struct crimp_gc_thread_t* next_thread;
} crimp_gc_thread_t;

extern thread_local crimp_gc_thread_t* _crimp_gc_thread;

// TODO: mark function type typedef

typedef struct crimp_type_t {
	// TODO: mark function
	const size_t size;
	
} crimp_type_t;

typedef struct crimp_gc_slot_internal* crimp_gc_slot;

// TYPES
/////////////////////////////////

/////////////////////////////////
// TESTING

void print_thread_list();

extern pthread_mutex_t _crimp_gc_log_mutex;
#define log(...) do{ \
    pthread_mutex_lock(&_crimp_gc_log_mutex); \
    _crimp_gc_thread != NULL ? fprintf(stderr, " (%02d) [%s] ", _crimp_gc_thread->thread_id, __func__) : fprintf(stderr, " (\?\?) [%s] ", __func__); \
    fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); \
    pthread_mutex_unlock(&_crimp_gc_log_mutex);}while(false)
// #define log(...) 

// TESTING
/////////////////////////////////

/////////////////////////////////
// INIT

void crimp_gc_init();
void crimp_gc_thread_register();
void crimp_gc_thread_unregister();

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

crimp_gc_handle_t* crimp_gc_handle_acquire(crimp_gc_slot* slot);
void crimp_gc_handle_release(crimp_gc_handle_t* handle);

crimp_gc_builder_t* crimp_gc_builder_create();
crimp_gc_handle_t* crimp_gc_builder_finish(crimp_gc_builder_t* builder);  // Invalidates builder

void crimp_gc_slot_set(crimp_gc_slot* dst, const void* src);  // Write barrier

// HANDLES
/////////////////////////////////

/////////////////////////////////
// FRAME

// these should probably be macros
void crimp_gc_frame_alloc();
crimp_gc_slot* crimp_gc_frame_push();
void crimp_gc_frame_dealloc(int n);

// FRAME
/////////////////////////////////

/////////////////////////////////
// COLLECT/MALLOC

void crimp_gc_collect();
void crimp_gc_malloc(crimp_type_t* type);
void crimp_gc_malloc_array(crimp_type_t* type, int count);

// COLLECT/MALLOC
/////////////////////////////////

#endif // CRIMP_GC