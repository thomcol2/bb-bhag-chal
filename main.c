#include <stdio.h>
#include <time.h>
#include "types.h"
#include "constants.h"
#include "utils.h"


// functions that generate moves for specific cases
u8 gen_goatplaces(BOARD goats, BOARD tigers, MOVE (*moves)[64]) {
    BOARD unoccupied = ~(goats | tigers);
    MOVE temp_move;
    u8 num_moves = 0;

    for (u8 i = 0; i < 25; i++) {
        // for every unoccupied position
        if ((unoccupied >> i) & 1) {
            // record move
            temp_move = GOAT_PLACE;
            temp_move |= i << 2;
            (*moves)[num_moves] = temp_move;
            num_moves++;
        }
    }

    return num_moves;
}

u8 gen_goatmoves(BOARD goats, BOARD tigers, MOVE (*moves)[64]) {
    BOARD unoccupied = ~(goats | tigers);
    BOARD legal_movements;
    MOVE temp_move;
    u8 num_moves = 0;

    for (u8 i = 0; i < 25; i++) {
        // for every goat in goats
        if ((goats >> i) & 1) {
            // check possible moves
            legal_movements = movelookup[i] & unoccupied;
            for (u8 u = 0; u < 25; u++) {
                // for every move
                if ((legal_movements >> u) & 1) {
                    // record move
                    temp_move = GOAT_MOVE;
                    temp_move |= i << 2; // target
                    temp_move |= u << 7; // destination
                    (*moves)[num_moves] = temp_move;
                    num_moves++;
                }
            }
        }
    }

    return num_moves;
}

u8 gen_tigermoves(BOARD goats, BOARD tigers, MOVE (*moves)[64]) {
    BOARD unoccupied = ~(goats | tigers);
    BOARD legal_movements, adjacent_goats;
    MOVE temp_move;
    u8 num_moves = 0;
    s8 opposite_idx;

    for (u8 i = 0; i < 25; i++) {
        // for every tiger in tigers
        if ((tigers >> i) & 1) {
            // check possible moves
            legal_movements = movelookup[i] & unoccupied;
            for (u8 u = 0; u < 25; u++) {
                // for every move
                if ((legal_movements >> u) & 1) {
                    // record move
                    temp_move = TIGER_MOVE;
                    temp_move |= i << 2; // target
                    temp_move |= u << 7; // destination
                    (*moves)[num_moves] = temp_move;
                    num_moves++;
                }
            }

            // check adjacent goats
            adjacent_goats = movelookup[i] & goats;
            if (adjacent_goats) {
                for (u8 u = 0; u < 25; u++) {
                    // for every goat in adjacent goats
                    if ((adjacent_goats >> u ) & 1) {
                        // calculate capture destination 
                        opposite_idx = (2 * u) - i;

                        // validate capture (inbounds, unoccupied)
                        if (opposite_idx >= 0 && 
                            opposite_idx <= 24 && 
                            (unoccupied & (1 << opposite_idx))) {
                            // record move
                            temp_move = TIGER_CAPTURE;
                            temp_move |= i << 2; // target
                            temp_move |= opposite_idx << 7; // destination
                            (*moves)[num_moves] = temp_move;
                            num_moves++;
                        }
                    }
                }
            }
        }
    }

    return num_moves;
}

u8 gen_moves(BOARD goats, BOARD tigers, STATE state, MOVE (*moves)[64]) {
    u8 turn = state & 1;
    u8 goats_captured = (state >> 1) & 7;
    u8 goats_placed = (state >> 4) & 31;

    if (goats_captured == 5)
        return 0;

    if (turn == TIGER_TURN) {
        return gen_tigermoves(goats, tigers, moves);
    } else if (goats_placed < 20) {
        return gen_goatplaces(goats, tigers, moves);
    } else {
        return gen_goatmoves(goats, tigers, moves);
    }
}

void make_move(BOARD *goats, BOARD *tigers, STATE *state, MOVE move) {
    // TODO
    //  - improve bit-wise operations on state variable 
    STATE new_state = 0;
    u8 move_type = move & 3;
    u8 target = (move >> 2) & 31;
    u8 destination = (move >> 7) & 31;
    u8 goats_captured = (*state >> 1) & 7;
    u8 goats_placed = (*state >> 4) & 31;


    switch (move_type) {
        case GOAT_MOVE:
            *goats ^= 1 << target;
            *goats |= 1 << destination;
            break;
        case TIGER_MOVE:
            *tigers ^= 1 << target;
            *tigers |= 1 << destination;
            break;
        case GOAT_PLACE:
            *goats |= 1 << target;
            goats_placed++;
            break;
        case TIGER_CAPTURE:
            *tigers ^= 1 << target;
            *tigers |= 1 << destination;
            *goats ^= 1 << ((destination + target) / 2);
            goats_captured++;
            break;
    }

    new_state ^= *state & 1;
    new_state |= goats_captured << 1;
    new_state |= goats_placed << 4;
    *state = new_state;
}

void undo_move(BOARD *goats, BOARD *tigers, STATE *state, MOVE move) {
    // TODO
    //  - improve bit-wise operations on state variable 
    STATE new_state = 0;
    u8 move_type = move & 3;
    u8 target = (move >> 2) & 31;
    u8 destination = (move >> 7) & 31;
    u8 goats_captured = (*state >> 1) & 7;
    u8 goats_placed = (*state >> 4) & 31;


    switch (move_type) {
        case GOAT_MOVE:
            *goats |= 1 << target;
            *goats ^= 1 << destination;
            break;
        case TIGER_MOVE:
            *tigers |= 1 << target;
            *tigers ^= 1 << destination;
            break;
        case GOAT_PLACE:
            *goats ^= 1 << target;
            goats_placed--;
            break;
        case TIGER_CAPTURE:
            *tigers |= 1 << target;
            *tigers ^= 1 << destination;
            *goats |= 1 << ((destination + target) / 2);
            goats_captured--;
            break;
    }

    new_state ^= *state & 1;
    new_state |= goats_captured << 1;
    new_state |= goats_placed << 4;
    *state = new_state;
}

u64 perft(BOARD goats, BOARD tigers, STATE state, u8 depth) {
    // TODO:
    //  - Add trackers for number of goats captured/games ended
    MOVE moves[64];
    u64 nodes = 0;
    u8 num_moves = gen_moves(goats, tigers, state, &moves);

    if (depth == 1)
        return (u64)num_moves;

    for (u8 i = 0; i < num_moves; i++) {
        make_move(&goats, &tigers, &state, moves[i]);
        nodes += perft(goats, tigers, state, depth - 1);
        undo_move(&goats, &tigers, &state, moves[i]);
    }

    return nodes;
}

void analyze_performance(u8 depth, u8 mode, u8 reps) {
    BOARD goats = GOAT_START;
    BOARD tigers = TIGER_START;
    STATE state = STATE_START;
    MOVE moves[64];

    clock_t begin, end;
    double t_sum = 0.0;

    printf("Mode: %u\n", mode);

    switch (mode) {
        case NODES:
            for (u8 i = 1; i <= depth; i++) {
                printf("%lu\n", perft(goats, tigers, state, i));
            }
            break;
        case NODES_TIMED:
            for (u8 i = 1; i <= depth; i++) {

                for (u8 u = 0; u < reps; u++) {
                    begin = clock();
                    perft(goats, tigers, state, i);
                    end = clock();
                    t_sum += (double)(end - begin) / CLOCKS_PER_SEC;
                }
                printf("%lf\n", t_sum / reps);
            }
            break;
    }
}

int main() {
    // TODO
    //  - find minimum possible size of move array
    //  - optimize functions in perft
    // LONG TERM
    //  - develop cli for game
    //  - develop ai

    analyze_performance(9, NODES, 1);

    return 0;
}
