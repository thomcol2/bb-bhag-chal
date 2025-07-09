#ifndef MOVES_H
#define MOVES_H

#include "constants.h"
#include "types.h"

unsigned char gen_goatplaces(Board goats, Board tigers, Move (*moves)[25]);
unsigned char gen_goatmoves(Board goats, Board tigers, Move (*moves)[25]);
unsigned char gen_tigermoves(Board goats, Board tigers, Move (*moves)[25]);

unsigned char gen_moves(Board goats, Board tigers, State state, Move (*moves)[25]);

void make_move(Board *goats, Board *tigers, State *state, Move move);
void undo_move(Board *goats, Board *tigers, State *state, Move move);
#endif
