#ifndef CONSTS_H
#define CONSTS_H

#include "types.h"

#define GOAT_START 0
#define TIGER_START 17825809
#define STATE_START 0

enum move_type {
    GOAT_MOVE,
    TIGER_MOVE,
    GOAT_PLACE,
    TIGER_CAPTURE,
};

extern const Board movelookup[25];
extern const Board capturelookup[25];

#endif
