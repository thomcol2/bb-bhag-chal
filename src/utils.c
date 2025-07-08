#include "utils.h"

void print_board(BOARD board) {
    printf("    A B C D E\n");

    for (u8 i = 0; i < 25; i++) {
        if ((i % 5) == 0) {
            printf("%u | ", (i/5) + 1);
        }
        
        printf("%u ", (board >> i) & 1);

        if (((i + 1) % 5) == 0)
            printf("\n");
    }
}

void print_game(BOARD goats, BOARD tigers) {
    printf("    A B C D E\n");

    for (u8 i = 0; i < 25; i++) {
        if ((i % 5) == 0) {
            printf("%u | ", (i/5) + 1);
        }
        
        if ((goats >> i) & 1) {
            printf("G ");
        } else if ((tigers >> i) & 1) {
            printf("T ");
        } else {
            printf("0 ");
        }


        if (((i + 1) % 5) == 0)
            printf("\n");
    }
}

void print_indices(void) {
    printf("    A  B  C  D  E\n");

    for (u8 i = 0; i < 25; i++) {
        if ((i % 5) == 0) {
            printf("%u | ", (i/5) + 1);
        }
        
        if (i < 10) {
            printf("%u  ", i);
        } else {
            printf("%u ", i);
        }

        if (((i + 1) % 5) == 0)
            printf("\n");
    }
}

void print_coord(u8 index) {
    u8 row = (index / 5) + 49; // convert row to ascii [12345]
    u8 column = (index % 5) + 65; // convert row to ascii [ABCDE]

    printf("%c%c", column, row);
}

void print_move(MOVE move) {
    u8 move_type = move & 3;

    switch (move_type) {
        case GOAT_MOVE:
            printf("G ");
            break;
        case TIGER_MOVE:
            printf("T ");
            break;
        case GOAT_PLACE:
            printf("G ");
            break;
        case TIGER_CAPTURE:
            printf("Tx");
            break;
    }

    if (move_type == GOAT_PLACE) {
        print_coord((move >> 2) & 31);
    } else {
        print_coord((move >> 2) & 31);
        print_coord((move >> 7) & 31);
    }
}

void print_state(STATE state) {
    printf("Turn %u, ", state & 1);
    printf("Captured %u, ", (state >> 1) & 7);
    printf("Placed, %u\n", (state >> 4) & 31);
}

void play_cli(void) {
    BOARD goats = GOAT_START;
    BOARD tigers = TIGER_START;
    STATE state = STATE_START;
    MOVE moves[25];
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

        // end game if 5 goats captured 
        if (((state >> 1) & 7) == 5) {
            printf("Tigers win!\n");
            break;
        }

        // end game if tigers have no moves
        if ((state & 1) && num_moves == 0) {
            printf("Goats win!\n");
            break;
        }
    }
}

u64 perft(BOARD goats, BOARD tigers, STATE state, u8 depth) {
    MOVE moves[25];
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

    clock_t begin, end;
    double t_sum = 0.0;


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
                    if (!u) { 
                        printf("%lu\n", perft(goats, tigers, state, i));
                    } else {
                        perft(goats, tigers, state, i);
                    }
                    end = clock();
                    t_sum += (double)(end - begin) / CLOCKS_PER_SEC;
                }
                printf("%lf\n", t_sum / reps);
            }
            break;
    }
}


void gen_movelookup(void) {
    s8 row, column;
    BOARD move_mask;

    for (u8 i = 0; i < 25; i++) {
        move_mask = 0;
        row = i / 5;
        column = i % 5;

        // calculate adjacent orthogonal positions
        if ( (column+1) <= 4)
            move_mask |= 1 << ((row * 5) + (column+1));
        if ( (column-1) >= 0)
            move_mask |= 1 << ((row * 5) + (column-1));
        if ( (row+1) <= 4)
            move_mask |= 1 << (((row+1) * 5) + column);
        if ( (row-1) >= 0)
            move_mask |= 1 << (((row-1) * 5) + column);

        // only calculate diagonals if the index is even
        if ( (i % 2) == 0) {
            if ( (column+1) <= 4 && (row+1) <= 4)
                move_mask |= 1 << (((row+1) * 5) + (column+1));
            if ( (column+1) <= 4 && (row-1) >= 0)
                move_mask |= 1 << (((row-1) * 5) + (column+1));
            if ( (column-1) >= 0 && (row+1) <= 4)
                move_mask |= 1 << (((row+1) * 5) + (column-1));
            if ( (column-1) >= 0 && (row-1) >= 0)
                move_mask |= 1 << (((row-1) * 5) + (column-1));
        }

        printf("%u,\n",move_mask);
        // print_board(move_mask);
    }
}

void gen_capturelookup(void) {
    BOARD adjacent, capture_mask;
    s8 dest_idx, tiger_row, tiger_column, adjacent_row, adjacent_column, dest_row, dest_column;

    for (u8 i = 0; i < 25; i++) {
        capture_mask = 0;
        adjacent = movelookup[i];

        for (u8 u = __builtin_ctz(adjacent); u < 25; u = __builtin_ctz(adjacent)) {
            dest_idx = (2 * u) - i;
            tiger_row = i / 5;
            tiger_column = i % 5;
            adjacent_row = u / 5;
            adjacent_column = u % 5;
            dest_row = dest_idx / 5;
            dest_column = dest_idx % 5;
            

           if ((tiger_row-adjacent_row)==(adjacent_row-dest_row) && 
                (tiger_column-adjacent_column)==(adjacent_column-dest_column)) {
                capture_mask |= (1 << dest_idx);
            }


           adjacent ^= 1 << u;
        }

        printf("%u,\n",capture_mask);
        // print_board(capture_mask);
    }
}

void gen_coordinatelookup(void) {
    for (u8 i = 0; i < 25; i++) { 
        printf("%u\n", 1 << i);
    }
}

u8 string_to_index(const char coord[3]) {
    u8 row = coord[1] % 49; // convert ascii to integer value (0-4)
    u8 column = coord[0] % 65; // convert ascii to integer value (0-4)

    // calculate index 
    u8 index = (row * 5 ) + column;
    return index;
}
