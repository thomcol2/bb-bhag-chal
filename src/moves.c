//
//  moves.c
//
//  REQUIRES:
//      gcc builtin: __builtin_ctz
//      constants.h
//      constants.c
//      types.h
//
//  FUNCTIONS:
//      gen_goatplaces
//      gen_goatmoves
//      gen_tigermoves
//      gen_moves
//      make_move
//      undo_move
//
//  PURPOSES:
//      To define functions that:
//          - generate possible moves given a board state
//          - perform a Move on a Board
//


#include "moves.h"


unsigned char
gen_goatplaces(Board goats, Board tigers, Move (*moves)[25])
{
    Board legal_moves = ~(goats | tigers);
    unsigned char num_moves = 0, i;
    
    for (i = __builtin_ctz(legal_moves); i < 25; i = __builtin_ctz(legal_moves)) {
        (*moves)[num_moves] = GOAT_PLACE | (i<<2);
        num_moves++;
        
        legal_moves &= legal_moves - 1;
    }

    return num_moves;
}

unsigned char
gen_goatmoves(Board goats, Board tigers, Move (*moves)[25])
{
    unsigned char num_moves = 0, i, u;
    const Board unoccupied = ~(goats | tigers);
    Board legal_moves;

    for (i = __builtin_ctz(goats); i < 25; i = __builtin_ctz(goats)) {
        legal_moves = movelookup[i] & unoccupied;
        if (legal_moves) {
            for (u = __builtin_ctz(legal_moves); u < 25; u = __builtin_ctz(legal_moves)) {
                (*moves)[num_moves] = GOAT_MOVE | (i<<2) | (u<<7);
                num_moves++;

                legal_moves &= legal_moves - 1;
            }
        }

        goats &= goats - 1;
    }

    return num_moves;
}

unsigned char
gen_tigermoves(Board goats, Board tigers, Move (*moves)[25])
{
    const Board unoccupied = ~(goats | tigers);
    Board legal_moves, adjacent_goats;
    unsigned char num_moves = 0;
    signed char dest_idx;

    for (unsigned char i = __builtin_ctz(tigers); i < 25; i = __builtin_ctz(tigers)) {
        legal_moves = movelookup[i] & unoccupied;
        if (legal_moves) {
            for (unsigned char u = __builtin_ctz(legal_moves); u < 25; u = __builtin_ctz(legal_moves)) {
                (*moves)[num_moves] = TIGER_MOVE | (i<<2) | (u<<7);
                num_moves++;

                legal_moves &= legal_moves - 1;
            }
        }

        adjacent_goats = movelookup[i] & goats;
        if (adjacent_goats) {
            for (unsigned char u = __builtin_ctz(adjacent_goats); u < 25; u = __builtin_ctz(adjacent_goats)) {
                dest_idx = (2 * u) - i;

                if ((1 << dest_idx) & capturelookup[i] & unoccupied & 33554431) {
                    (*moves)[num_moves] = TIGER_CAPTURE | (i<<2) | (dest_idx<<7);
                    num_moves++;
                }

               adjacent_goats &= adjacent_goats - 1;
            }
        }
       
        tigers &= tigers - 1;
    }

    return num_moves;
}

unsigned char
gen_moves(Board goats, Board tigers, State state, Move (*moves)[25])
{
    if (((state >> 1) & 7) == 5)
        return 0;

    if (state & 1) {
        return gen_tigermoves(goats, tigers, moves);
    } else if (((state >> 4) & 31) < 20) {
        return gen_goatplaces(goats, tigers, moves);
    } else {
        return gen_goatmoves(goats, tigers, moves);
    }
}

void
make_move(Board *goats, Board *tigers, State *state, Move move)
{
    const unsigned char move_type = move & 3;
    const unsigned char target = (move >> 2) & 31;
    const unsigned char dest = (move >> 7) & 31;

    switch (move_type) {
        case GOAT_MOVE:
            *goats ^= (1 << target) | (1 << dest);
            break;
        case TIGER_MOVE:
            *tigers ^= (1 << target) | (1 << dest);
            break;
        case GOAT_PLACE:
            *goats ^= 1 << target;
            *state = (*state & ~(31 << 4)) | ((((*state >> 4) & 31) + 1) << 4); 
            break;
        case TIGER_CAPTURE:
            *tigers ^= (1 << target) | (1 << dest);
            *goats ^= 1 << ((dest + target) / 2);
            *state = (*state & ~(7 << 1)) | ((((*state >> 1) & 7) + 1) << 1); 
            break;
    }

    *state ^= 1;
}

void 
undo_move(Board *goats, Board *tigers, State *state, Move move) 
{
    const unsigned char move_type = move & 3;
    const unsigned char target = (move >> 2) & 31;
    const unsigned char dest = (move >> 7) & 31;

    switch (move_type) {
        case GOAT_MOVE:
            *goats ^= (1 << target) | (1 << dest);
            break;
        case TIGER_MOVE:
            *tigers ^= (1 << target) | (1 << dest);
            break;
        case GOAT_PLACE:
            *goats ^= 1 << target;
            *state = (*state & ~(31 << 4)) | ((((*state >> 4) & 31) - 1) << 4); 
            break;
        case TIGER_CAPTURE:
            *tigers ^= (1 << target) | (1 << dest);
            *goats ^= 1 << ((dest + target) / 2);
            *state = (*state & ~(7 << 1)) | ((((*state >> 1) & 7) + 1) << 1); 
            break;
    }

    *state ^= 1;
}
