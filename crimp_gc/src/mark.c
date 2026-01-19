#include <stdio.h>
#include <stdlib.h>

#include "crimpGc.h"
#include "internalTypes.h"
#include "mark.h"


void crimpGc_mark(crimpGc_grayList_t* grayList, crimpGc_typeHeader_t* obj)
{
    (void)grayList;
    (void)obj;

    // TODO: if obj is white, mark to gray, and add to graylist
    // Hmm... need to give a different name between simply graying and adding to graylist, vs popping from graylist, checking the children, and marking as black

    // not implemented
    fprintf(stderr, "%s not implemented", __func__);
    exit(1);
}