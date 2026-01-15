#ifndef SHADOW_STACK
#define SHADOW_STACK


////////////////////////////////////////
// TYPES

struct crimpGc_shadowStack_frame_t;

typedef struct crimpGc_shadowStack_frame_t {
    int roots_count;
    void** roots;
    struct crimpGc_shadowStack_frame_t* next_frame;
    struct crimpGc_shadowStack_frame_t* prev_frame;
} crimpGc_shadowStack_frame_t;

// TODO: remove these, as I believe they aren't needed
//extern crimpGc_shadowStack_frame_t _crimpGc_shadowStack_head;
//extern crimpGc_shadowStack_frame_t* _crimpGc_shadowStack_tail;

// TYPES
////////////////////////////////////////

crimpGc_shadowStack_frame_t* crimpGc_shadowStack_frame_push(int roots_count);
void crimpGc_shadowStack_frame_pop(crimpGc_shadowStack_frame_t* frame);

#endif // SHADOW_STACK