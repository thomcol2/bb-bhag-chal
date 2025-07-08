#ifndef MOVES_H
#define MOVES_H

#include "constants.h"
#include "types.h"

u8 gen_goatplaces(BOARD goats, BOARD tigers, MOVE (*moves)[25]);
u8 gen_goatmoves(BOARD goats, BOARD tigers, MOVE (*moves)[25]);
u8 gen_tigermoves(BOARD goats, BOARD tigers, MOVE (*moves)[25]);

u8 gen_moves(BOARD goats, BOARD tigers, STATE state, MOVE (*moves)[25]);

void make_move(BOARD *goats, BOARD *tigers, STATE *state, MOVE move);
void undo_move(BOARD *goats, BOARD *tigers, STATE *state, MOVE move);
#endif
