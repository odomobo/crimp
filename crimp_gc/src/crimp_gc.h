#ifndef CRIMP_GC
#define CRIMP_GC

// TODO: mark function type typedef

typedef struct {
	// TODO: mark function
	const size_t size;
	
} crimp_type;

typedef struct crimp_gc_slot_internal* crimp_gc_slot;

/////////////////////////////////
// HANDLES

typedef struct {
    const void* data;  // Read-only access
    int _refcount;
} crimp_gc_handle;

typedef struct {
    crimp_gc_slot slot;
    int _refcount;
} crimp_gc_builder;

crimp_gc_handle* crimp_gc_handle_acquire(crimp_gc_slot* slot);
void crimp_gc_handle_release(crimp_gc_handle* handle);

crimp_gc_builder* crimp_gc_builder_create();
crimp_gc_handle* crimp_gc_builder_finish(crimp_gc_builder* builder);  // Invalidates builder

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
void crimp_gc_malloc(crimp_type* type);
void crimp_gc_malloc_array(crimp_type* type, int count);

// COLLECT/MALLOC
/////////////////////////////////

#endif // CRIMP_GC