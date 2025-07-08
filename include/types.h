#ifndef TYPES_H
#define TYPES_H

// 25 bits for each position on the board
typedef unsigned int BOARD; 

// 2 bits for move type (see move_type enum in constants.h)
// 5 bits for target
// 5 bits for destination
typedef unsigned short MOVE;

// 1 bit for turn (0: G, 1: T)
// 3 bits for number of captured goats
// 5 bits for number of goats placed
typedef unsigned short STATE;

typedef unsigned long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;
#endif
