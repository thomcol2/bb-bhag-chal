#ifndef ENGINE_H
#define ENGINE_H

#include "constants.h"
#include "moves.h"
#include "types.h"

int evaluate(Board goats, Board tigers, State state);
int minimize(Board goats, Board tigers, State state, unsigned char depth);
int maximize(Board goats, Board tigers, State state, unsigned char depth);
Move minimax(Board goats, Board tigers, State state, unsigned char depth);
#endif
