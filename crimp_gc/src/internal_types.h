#ifndef CRIMPGC_INTERNAL_TYPES
#define CRIMPGC_INTERNAL_TYPES

#include "crimpGc.h"

enum crimpGc_color {
    WHITE,
    GRAY,
    BLACK,
};

struct crimpGc_internal_typeHeader_t {
    crimp_type_t* type;
    enum crimpGc_color color;

} crimpGc_internal_typeHeader_t;

struct crimpGc_slot_internal_t {
    crimpGc_internal_typeHeader_t* object;
} crimpGc_slot_internal_t;

#endif // CRIMPGC_INTERNAL_TYPES
