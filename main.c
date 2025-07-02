#include <stdio.h>
#include <time.h>
#include "types.h"
#include "constants.h"
#include "utils.h"

u8 gen_goatplaces(BOARD goats, BOARD tigers, MOVE (*moves)[32]) {
    BOARD unoccupied = ~(goats | tigers);
    u8 num_moves = 0;
    u8 i;
    
    for (i = __builtin_ffs(unoccupied); i && i <= 25; i = __builtin_ffs(unoccupied)) {
        i--;

        (*moves)[num_moves] = GOAT_PLACE | (i<<2);
        num_moves++;
        
        unoccupied ^= 1 << i;
    }

    return num_moves;
}

u8 gen_goatmoves(BOARD goats, BOARD tigers, MOVE (*moves)[32]) {
    BOARD unoccupied = ~(goats | tigers);
    BOARD legal_moves;
    u8 num_moves = 0;
    u8 i, u;

    for (i = __builtin_ffs(unoccupied); i && i <= 25; i = __builtin_ffs(unoccupied)) {
        i--;

        // for every legal move
        legal_moves = movelookup[i] & unoccupied;
        for (u = __builtin_ffs(legal_moves); u && u <= 25; u = __builtin_ffs(legal_moves)) {
            u--;

            // record move
            (*moves)[num_moves] = GOAT_MOVE | (i<<2) | (u<<7);
            num_moves++;

            legal_moves ^= 1 << u;
        }

        unoccupied ^= 1 << i;
    }

    return num_moves;
}

u8 gen_tigermoves(BOARD goats, BOARD tigers, MOVE (*moves)[32]) {
    BOARD unoccupied = ~(goats | tigers);
    BOARD legal_moves, adjacent_goats;
    u8 num_moves = 0;
    u8 i, u;
    s8 dest_idx;

    for (i = __builtin_ffs(tigers); i && i <= 25; i = __builtin_ffs(tigers)) {
        i--;

        // for every legal move
        legal_moves = movelookup[i] & unoccupied;
        for (u = __builtin_ffs(legal_moves); u && u <= 25; u = __builtin_ffs(legal_moves)) {
            u--;

            // record move
            (*moves)[num_moves] = TIGER_MOVE | (i<<2) | (u<<7);
            num_moves++;

            legal_moves ^= 1 << u;
        }

        // for every adjacent goat
        adjacent_goats = movelookup[i] & goats;
        for (u = __builtin_ffs(adjacent_goats); u && u <= 25; u = __builtin_ffs(adjacent_goats)) {
            u--;

            // calculate capture destination 
            dest_idx = (2 * u) - i;

            // validate capture
            if (dest_idx >= 0 && dest_idx <= 24 && (unoccupied & (1 << dest_idx))) {
                // record capture
                (*moves)[num_moves] = TIGER_CAPTURE | (i<<2) | (dest_idx<<7);
                num_moves++;
            }

           adjacent_goats ^= 1 << u;
        }
       
        tigers ^= 1 << i;
    }

    return num_moves;
}

u8 gen_moves(BOARD goats, BOARD tigers, STATE state, MOVE (*moves)[32]) {
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
            for (u8 i = start; i <= depth; i++) {
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
    //  - optimize performance 
    //  - develop ai

    // analyze_performance(1, 7, TIMED, 20);
    // analyze_performance(8, 8, TIMED, 5);
    // analyze_performance(9, 9, TIMED, 1);

    play_cli();

    return 0;
}
