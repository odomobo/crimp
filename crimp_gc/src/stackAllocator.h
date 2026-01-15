#ifndef STACK_ALLOCATOR
#define STACK_ALLOCATOR

// The way this should work is you call malloc and free from it, and it will return you some mmeory, but you have to call malloc and free in a stack-like fashion, otherwise the application crashes.
// The way it works is it allocates large blocks of memory, and when you call alloc, it tries to give memory from the current block, if possible. If not, it allocates another block.
// When freeing the last memory from a block, instead of freeing that block, it puts it into the "free block cache", which is only ever a single block. If there's already a block in the free block cache, it will free that block first.


#endif // STACK_ALLOCATOR