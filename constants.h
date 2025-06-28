#ifndef _CONSTS_H_
#define _CONSTS_H_

#include "types.h"

enum board_states {
    GOAT_START = 0,
    TIGER_START = 17825809,
};

enum move_types {
    GOAT_MOVE = 0,
    TIGER_MOVE = 1,
    GOAT_PLACE = 2,
    TIGER_CAPTURE = 3,
};

enum game_states {
    GOAT_TURN = 0,
    TIGER_TURN = 1,
    STATE_START = 0,
};

enum analysis_modes {
    NODES = 0,
    NODES_TIMED = 1,
};

enum coordinates_to_indices {
    A1, B1, C1, D1, E1,
    A2, B2, C2, D2, E2,
    A3, B3, C3, D3, E3,
    A4, B4, C4, D4, E4,
    A5, B5, C5, D5, E5,
};

const BOARD movelookup[25] = {
    98,
    69,
    458,
    276,
    776,
    1089,
    7335,
    4420,
    29340,
    16656,
    100448,
    70720,
    469440,
    282880,
    795392,
    1115136,
    7511040,
    4526080,
    30044160,
    17055744,
    2195456,
    5308416,
    10944512,
    21233664,
    9175040,
};
#endif
