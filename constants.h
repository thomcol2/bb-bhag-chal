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
    NODES,
    TIMED,
    NODES_TIMED,
};

enum coordinates_to_boards {
    A1 = 1,
    B1 = 2,
    C1 = 4,
    D1 = 8,
    E1 = 16,
    A2 = 32,
    B2 = 64,
    C2 = 128,
    D2 = 256,
    E2 = 512,
    A3 = 1024,
    B3 = 2048,
    C3 = 4096,
    D3 = 8192,
    E3 = 16384,
    A4 = 32768,
    B4 = 65536,
    C4 = 131072,
    D4 = 262144,
    E4 = 524288,
    A5 = 1048576,
    B5 = 2097152,
    C5 = 4194304,
    D5 = 8388608,
    E5 = 16777216,
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
