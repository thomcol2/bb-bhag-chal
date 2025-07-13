#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#include "moves.h"
#include "types.h"

void print_board(Board board);
void print_game(Board goats, Board tigers);
void print_coord(unsigned char index);
void print_move(Move move);
void print_state(State state);

void gen_movelookup(void);
void gen_capturelookup(void);
#endif
