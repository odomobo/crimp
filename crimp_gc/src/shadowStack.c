#include "crimpGc.h"
#include "shadowStack.h"

#include <stdlib.h>


////////////////////////////////////////
// GLOBALS

crimpGc_shadowStack_frame_t _crimpGc_shadowStack_head = {
    .roots_count = 0,    // unused for head
    .roots = NULL,       // unused for head
    .next_frame = NULL,  // next_frame and prev_frame and the only things that matter for head
    .prev_frame = NULL,
};
crimpGc_shadowStack_frame_t* _crimpGc_shadowStack_tail = &_crimpGc_shadowStack_head;

// GLOBALS
////////////////////////////////////////

// TODO: eventually make shadow stack more efficient, so a single allocated block of memory can hold many frames.

crimpGc_shadowStack_frame_t* crimpGc_shadowStack_frame_push(int roots_count) {
    // TODO: use stackAllocator
    crimpGc_shadowStack_frame_t* frame = calloc(sizeof(crimpGc_shadowStack_frame_t), 1);
    crimpGc_assert(frame != NULL);
    frame->roots = calloc(sizeof(void*), roots_count);
    crimpGc_assert(frame->roots != NULL);
    frame->roots_count = roots_count;
    frame->next_frame = NULL;
    frame->prev_frame = _crimpGc_shadowStack_tail;
    
    _crimpGc_shadowStack_tail->next_frame = frame;

    return frame;
}

void crimpGc_shadowStack_frame_pop(crimpGc_shadowStack_frame_t* frame) {
    crimpGc_assert(frame != NULL);

    // if we're not popping the tail, something bad has happened
    crimpGc_assert(frame == _crimpGc_shadowStack_tail);

    // update tail
    crimpGc_assert(_crimpGc_shadowStack_tail->prev_frame != NULL);
    _crimpGc_shadowStack_tail = _crimpGc_shadowStack_tail->prev_frame;
    _crimpGc_shadowStack_tail->next_frame = NULL;

    // free stuff
    crimpGc_assert(frame->roots != NULL);
    free(frame->roots);
    free(frame);
}