#ifndef CRIMPGC_MARK
#define CRIMPGC_MARK

#include "crimpGc.h"
#include "internalTypes.h"


void crimpGc_mark_gray(crimpGc_grayList_t* grayList, crimpGc_typeHeader_t* obj);
void crimpGc_mark_black(crimpGc_grayList_t* grayList, crimpGc_typeHeader_t* obj);

#endif // CRIMPGC_MARK