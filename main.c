#include <stdio.h>
#include <time.h>
#include "types.h"
#include "constants.h"
#include "utils.h"


// functions that generate moves for specific cases
u8 gen_goatplaces(BOARD goats, BOARD tigers, MOVE (*moves)[32]) {
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

u8 gen_goatmoves(BOARD goats, BOARD tigers, MOVE (*moves)[32]) {
    BOARD unoccupied = ~(goats | tigers);
    BOARD legal_move_mask;
    MOVE temp_move;
    u8 num_moves = 0;

    for (u8 i = 0; i < 25; i++) {
        // for every goat in goats
        if ((goats >> i) & 1) {
            // check possible moves
            legal_move_mask = movelookup[i] & unoccupied;
            for (u8 u = 0; u < 25; u++) {
                // for every move
                if ((legal_move_mask >> u) & 1) {
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

u8 gen_tigermoves(BOARD goats, BOARD tigers, MOVE (*moves)[32]) {
    BOARD unoccupied = ~(goats | tigers);
    BOARD legal_move_mask, adjacent_goats;
    MOVE temp_move;
    u8 num_moves = 0;
    s8 opposite_idx;

    for (u8 i = 0; i < 25; i++) {
        // for every tiger in tigers
        if ((tigers >> i) & 1) {
            // check possible moves
            legal_move_mask = movelookup[i] & unoccupied;
            for (u8 u = 0; u < 25; u++) {
                // for every move
                if ((legal_move_mask >> u) & 1) {
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

                        // check if capture destination is inbounds and unoccupied
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

u8 gen_moves(BOARD goats, BOARD tigers, STATE state, MOVE (*moves)[32]) {
    u8 turn = state & 1;
    u8 goats_captured = (state >> 1) & 7;
    u8 goats_placed = (state >> 4) & 31;

    if (goats_captured == 5)
        return 0;

    if (turn == TIGER_TURN)
        return gen_tigermoves(goats, tigers, moves);
    if (goats_placed < 20) 
        return gen_goatplaces(goats, tigers, moves);

    return gen_goatmoves(goats, tigers, moves);
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

u64 perft(BOARD goats, BOARD tigers, STATE state, u8 depth) {
    // TODO:
    //  - Add trackers for number of goats captured/games ended
    MOVE moves[32];
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

void analyze_performance(u8 start, u8 depth, u8 mode, u8 reps) {
    BOARD goats = GOAT_START; 
    BOARD tigers = TIGER_START;
    STATE state = STATE_START;
    MOVE moves[32];

    clock_t begin, end;
    double t_sum = 0.0;

    u64 result;

    if (start > depth) {
        printf("Invalid start value: greater than depth\n");
        return;
    }

    printf("Depths %u-%u, Mode %u, Reps %u\n", start, depth, mode, reps);

    switch (mode) {
        case NODES:
            for (u8 i = start; i <= depth; i++) {
                printf("%lu\n", perft(goats, tigers, state, i));
            }
            break;
        case TIMED:
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
        case NODES_TIMED:
            for (u8 i = start; i <= depth; i++) {
                for (u8 u = 0; u < reps; u++) {
                    begin = clock();
                    result = perft(goats, tigers, state, i);
                    end = clock();
                    t_sum += (double)(end - begin) / CLOCKS_PER_SEC;
                }
                printf("%lu\n", result);
                printf("%lf\n", t_sum / reps);
            }
            break;
    }
}

void play_cli(void) {
    BOARD goats = GOAT_START;
    BOARD tigers = TIGER_START;
    STATE state = STATE_START;
    MOVE moves[32];
    u8 input, num_moves;

    while (1) {
        // print board
        printf("\n");
        print_state(state);
        print_game(goats, tigers); 

        // offer list of moves
        num_moves = gen_moves(goats, tigers, state, &moves);
        for (u8 i = 0; i < num_moves; i++) {
            printf("%u: ", i);
            print_move(moves[i]);
            printf("\n");
        }

        // get move from command line
        get_input:
            printf("Select move: ");

            if (!scanf("%hhu", &input)) {
                printf("Invalid input format\n");
                while ((getchar()) != '\n'); // flush stdin
                goto get_input;
            }

            if (input >= num_moves) {
                printf("Move must be in range 0-%u\n", num_moves - 1);
                while ((getchar()) != '\n'); // flush stdin
                goto get_input;
            }

        // apply move
        make_move(&goats, &tigers, &state, moves[input]);

        // check if game ends
        if (((state >> 1) & 7) == 5) {
            printf("Tigers win!\n");
            break;
        }
        if ((state & 1) && num_moves == 0) {
            printf("Goats win!\n");
            break;
        }
    }
}

int main() {
    // TODO
    //  - optimize performance of functions 
    //  - develop ai

    play_cli();
    // analyze_performance(1, 7, NODES_TIMED, 1);

    return 0;
}
