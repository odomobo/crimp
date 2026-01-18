#include <stdlib.h>
#include <stdio.h>

#include "crimpGc.h"
#include "shadowStack.h"
#include "mark.h"


////////////////////////////////////////
// GLOBALS

crimpGc_shadowStack_frame_t* _crimpGc_shadowStack_head = NULL;

// GLOBALS
////////////////////////////////////////

// TODO: eventually make shadow stack more efficient, so a single allocated block of memory can hold many frames.
// Once we implement stackAllocator, we can try using stack allocator (although you should be able to change implementation with preprocessor definitions)

crimpGc_shadowStack_frame_t* crimpGc_shadowStack_frame_push(int roots_count) {
    crimpGc_shadowStack_frame_t* frame = calloc(sizeof(crimpGc_shadowStack_frame_t), 1);
    crimpGc_assert(frame != NULL);

    frame->roots = calloc(sizeof(void*), roots_count);
    crimpGc_assert(frame->roots != NULL);

    frame->roots_count = roots_count;
    frame->prev_frame = _crimpGc_shadowStack_head;

    // update head
    _crimpGc_shadowStack_head = frame;

    return frame;
}

void crimpGc_shadowStack_frame_pop(crimpGc_shadowStack_frame_t* frame) {
    crimpGc_assert(frame != NULL);

    // if we're not popping the head, something bad has happened
    crimpGc_assert(frame == _crimpGc_shadowStack_head);

    // free stuff
    crimpGc_assert(frame->roots != NULL);
    free(frame->roots);
    free(frame);
    
    // update head
    _crimpGc_shadowStack_head = _crimpGc_shadowStack_head->prev_frame;
}

crimpGc_slot_t* crimpGc_shadowStack_frame_getSlot(crimpGc_shadowStack_frame_t* frame, int slot_index)
{
    crimpGc_assert(slot_index >= 0 && slot_index < frame->roots_count);
    return &frame->roots[slot_index];
}

static void crimpGc_shadowStack_frame_markRoots(crimpGc_grayList_t* grayList, crimpGc_shadowStack_frame_t* frame) {
    // iterate over all roots and add to gray list

    for (int i = 0; i < frame->roots_count; i++)
    {
        crimpGc_mark(grayList, &frame->roots[i]);
    }
}

void crimpGc_shadowStack_markRoots(crimpGc_grayList_t* grayList) {
    // iterate over all frames and add to gray list
    
    for (
        crimpGc_shadowStack_frame_t* f = _crimpGc_shadowStack_head; 
        f != NULL; 
        f = f->prev_frame
    )
    {
        crimpGc_shadowStack_frame_markRoots(grayList, f);
    }
}