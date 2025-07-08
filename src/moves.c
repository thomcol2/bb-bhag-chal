#include "moves.h"

u8 gen_goatplaces(BOARD goats, BOARD tigers, MOVE (*moves)[25]) {
    BOARD unoccupied = ~(goats | tigers);
    u8 num_moves = 0;
    u8 i;
    
    for (i = __builtin_ctz(unoccupied); i < 25; i = __builtin_ctz(unoccupied)) {
        (*moves)[num_moves] = GOAT_PLACE | (i<<2);
        num_moves++;
        
        unoccupied &= unoccupied - 1;
    }

    return num_moves;
}

u8 gen_goatmoves(BOARD goats, BOARD tigers, MOVE (*moves)[25]) {
    BOARD unoccupied = ~(goats | tigers);
    BOARD legal_moves;
    u8 num_moves = 0;
    u8 i, u;

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

u8 gen_tigermoves(BOARD goats, BOARD tigers, MOVE (*moves)[25]) {
    BOARD unoccupied = ~(goats | tigers);
    BOARD legal_moves, adjacent_goats;
    u8 num_moves = 0;
    u8 i, u;
    s8 dest_idx;

    for (i = __builtin_ctz(tigers); i < 25; i = __builtin_ctz(tigers)) {
        legal_moves = movelookup[i] & unoccupied;
        if (legal_moves) {
            for (u = __builtin_ctz(legal_moves); u < 25; u = __builtin_ctz(legal_moves)) {
                (*moves)[num_moves] = TIGER_MOVE | (i<<2) | (u<<7);
                num_moves++;

                legal_moves &= legal_moves - 1;
            }
        }

        adjacent_goats = movelookup[i] & goats;
        if (adjacent_goats) {
            for (u = __builtin_ctz(adjacent_goats); u < 25; u = __builtin_ctz(adjacent_goats)) {
                dest_idx = (2 * u) - i;

                // if destination is a valid capture and unoccupied
                // large integer is a mask for the first 25 bits
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

u8 gen_moves(BOARD goats, BOARD tigers, STATE state, MOVE (*moves)[25]) {
    if (((state >> 1) & 7) == 5)
        return 0;

    if ((state & 1) == TIGER_TURN) {
        return gen_tigermoves(goats, tigers, moves);
    } else if (((state >> 4) & 31) < 20) {
        return gen_goatplaces(goats, tigers, moves);
    } else {
        return gen_goatmoves(goats, tigers, moves);
    }
}

void make_move(BOARD *goats, BOARD *tigers, STATE *state, MOVE move) {
    u8 goats_placed, goats_captured;
    u8 move_type = move & 3;
    u8 target = (move >> 2) & 31;
    u8 destination = (move >> 7) & 31;

    switch (move_type) {
        case GOAT_MOVE:
            *goats ^= 1 << target;
            *goats ^= 1 << destination;
            break;
        case TIGER_MOVE:
            *tigers ^= 1 << target;
            *tigers ^= 1 << destination;
            break;
        case GOAT_PLACE:
            *goats ^= 1 << target;
            goats_placed = (*state >> 4) & 31; // read counter bits 
            *state &= ~(31 << 4);              // clear counter bits
            *state |= (goats_placed + 1) << 4; // increment counter 
            break;
        case TIGER_CAPTURE:
            *tigers ^= 1 << target;
            *tigers ^= 1 << destination;
            *goats ^= 1 << ((destination + target) / 2);
            goats_captured = (*state >> 1) & 7;  // read counter bits
            *state &= ~(7 << 1);                 // clear counter bits
            *state |= (goats_captured + 1) << 1; // increment counter
            break;
    }

    *state ^= 1; // toggle turn bit
}

void undo_move(BOARD *goats, BOARD *tigers, STATE *state, MOVE move) {
    u8 goats_placed, goats_captured;
    u8 move_type = move & 3;
    u8 target = (move >> 2) & 31;
    u8 destination = (move >> 7) & 31;

    switch (move_type) {
        case GOAT_MOVE:
            *goats ^= 1 << target;
            *goats ^= 1 << destination;
            break;
        case TIGER_MOVE:
            *tigers ^= 1 << target;
            *tigers ^= 1 << destination;
            break;
        case GOAT_PLACE:
            *goats ^= 1 << target;
            goats_placed = (*state >> 4) & 31; // read counter bits 
            *state &= ~(31 << 4);              // clear counter bits
            *state |= (goats_placed - 1) << 4; // decrement counter 
            break;
        case TIGER_CAPTURE:
            *tigers ^= 1 << target;
            *tigers ^= 1 << destination;
            *goats ^= 1 << ((destination + target) / 2);
            goats_captured = (*state >> 1) & 7;  // read counter bits
            *state &= ~(7 << 1);                 // clear counter bits
            *state |= (goats_captured - 1) << 1; // decrement counter
            break;
    }

    *state ^= 1; // toggle turn bit
}
