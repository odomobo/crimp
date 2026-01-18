#include <stdio.h>
#include <stdlib.h>

#include "crimpGc.h"


void crimpGc_mark(crimpGc_grayList_t* grayList, crimpGc_slot_t* slot)
{
    (void)grayList;
    (void)slot;

    // not implemented
    fprintf(stderr, "%s not implemented", __func__);
    exit(1);
}