#ifndef TYPES_H
#define TYPES_H

// 25 bits for 25 positions on the board (0: unoccupied, 1: occupied)
typedef unsigned int Board; 

// 1 bit for turn          (0: G, 1: T)
// 3 bits for number of captured goats
// 5 bits for number of goats placed
typedef unsigned short State;

// 2 bits for move type    (see "enum move_type" in constants.h)
// 5 bits for target       (represented as an index 0-24 into a Board type)
// 5 bits for destination  (represented as an index 0-24 into a Board type)
typedef unsigned short Move;
#endif
