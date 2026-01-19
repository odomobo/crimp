#ifndef CRIMPGC_GRAYLIST
#define CRIMPGC_GRAYLIST

#include "crimpGc.h"
#include "internalTypes.h"

void crimpGc_grayList_push(crimpGc_grayList_t* grayList, crimpGc_typeHeader_t* obj);
crimpGc_typeHeader_t* crimpGc_grayList_pop(crimpGc_grayList_t* grayList);

#endif // CRIMPGC_GRAYLIST