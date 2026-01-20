#include <stdio.h>
#include <stdlib.h>

#include "crimpGc.h"
#include "grayList.h"

#define CRIMPGC_GRAYLIST_BLOCK_SIZE 1000

typedef struct crimpGc_grayList_block_t {
    crimpGc_slot_t items[CRIMPGC_GRAYLIST_BLOCK_SIZE];
    int count;
    struct crimpGc_grayList_block_t* next;
} crimpGc_grayList_block_t;

typedef struct crimpGc_grayList_t {
    crimpGc_grayList_block_t* head;
} crimpGc_grayList_t;


crimpGc_grayList_t* crimpGc_grayList_create() {
    crimpGc_grayList_t* ret = calloc(sizeof(crimpGc_grayList_t), 1);
    crimpGc_assert(ret != NULL);
    ret->head = calloc(sizeof(crimpGc_grayList_block_t), 1);
    crimpGc_assert(ret->head != NULL);
    ret->head->count = 0;
    ret->head->next = NULL;
    return ret;
}

void crimpGc_grayList_push(crimpGc_grayList_t* grayList, crimpGc_typeHeader_t* obj) {
    (void)grayList;
    (void)obj;

    fprintf(stderr, "%s not implemented", __func__);
    exit(1);
}

crimpGc_typeHeader_t* crimpGc_grayList_pop(crimpGc_grayList_t* grayList) {
    (void)grayList;

    fprintf(stderr, "%s not implemented", __func__);
    exit(1);
}

// TODO: crimpGc_grayList_mergeInto(dst, src)