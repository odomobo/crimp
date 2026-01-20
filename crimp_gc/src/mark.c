#include <stdio.h>
#include <stdlib.h>

#include "crimpGc.h"
#include "internalTypes.h"
#include "mark.h"


void crimpGc_mark_gray(crimpGc_grayList_t* grayList, crimpGc_typeHeader_t* obj)
{
    (void)grayList;
    (void)obj;

    // TODO: if obj is white, mark to gray atomically, and add to graylist

    // not implemented
    fprintf(stderr, "%s not implemented", __func__);
    exit(1);
}

void crimpGc_mark_black(crimpGc_grayList_t* grayList, crimpGc_typeHeader_t* obj)
{
    (void)grayList;
    (void)obj;

    // TODO: assert obj is gray (if it was already black, then that means that somehow it was added to the gray list twice, which should be a bug)
    // TODO: mark children as gray (by calling mark_fn, if there is one)
    // TODO: mark as black, atomically
    
    // not implemented
    fprintf(stderr, "%s not implemented", __func__);
    exit(1);
}