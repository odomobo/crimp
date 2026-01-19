#ifndef CRIMPGC_INTERNAL_TYPES
#define CRIMPGC_INTERNAL_TYPES

#include "crimpGc.h"

enum crimpGc_color {
    WHITE,
    GRAY,
    BLACK,
};

typedef struct crimpGc_typeHeader_t {
    crimp_type_t* type;
    enum crimpGc_color color;
} crimpGc_typeHeader_t;

// we want to obscure slot types, to prevent their misuse
typedef struct crimpGc_slot_internal_t {
    crimpGc_typeHeader_t typeHeader;
} crimpGc_slot_internal_t;

#endif // CRIMPGC_INTERNAL_TYPES
