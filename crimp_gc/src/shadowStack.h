#ifndef CRIMPGC_SHADOW_STACK
#define CRIMPGC_SHADOW_STACK

#include "crimpGc.h"


////////////////////////////////////////
// TYPES

typedef struct crimpGc_shadowStack_frame_t {
    int roots_count;
    crimpGc_slot_t* roots;
    struct crimpGc_shadowStack_frame_t* prev_frame;
} crimpGc_shadowStack_frame_t;

// TYPES
////////////////////////////////////////

crimpGc_shadowStack_frame_t* crimpGc_shadowStack_frame_push(int roots_count);
void crimpGc_shadowStack_frame_pop(crimpGc_shadowStack_frame_t* frame);
crimpGc_slot_t* crimpGc_shadowStack_frame_getSlot(crimpGc_shadowStack_frame_t* frame, int slot_index);
void crimpGc_shadowStack_markRoots(crimpGc_grayList_t* grayList);

#endif // CRIMPGC_SHADOW_STACK