//
//  utils.c 
//
//  REQUIRES:
//      gcc builtin: __builtin_ctz
//      stdio.h
//      time.h
//      engine.h
//      engine.c
//      moves.h
//      moves.c
//      types.h
//
//  FUNCTIONS:
//		print_board
//		print_game
//		print_coord
//		print_move
//		print_state
//		gen_movelookup
//		gen_capturelookup
//		perft
//		analyze_perft
//		play_cli
//
//  PURPOSES:
//      To define functions that:
//          - evaluate the state of a game
//          - search for the optimal move
//

#include "utils.h"

void 
print_board(Board board)
{
    unsigned char i;

    //print column letters
    printf("    A B C D E\n");

    for (i = 0; i < 25; i++) {
        if ((i % 5) == 0)
            // print row number
            printf("%u | ", (i/5) + 1);
        
        // print value at ith bit of board
        printf("%u ", (board >> i) & 1);

        if (((i + 1) % 5) == 0)
            // newline at end of row
            printf("\n");
    }
}

void
print_game(Board goats, Board tigers)
{
    unsigned char i;

    //print column letters
    printf("    A B C D E\n");

    for (i = 0; i < 25; i++) {
        if ((i % 5) == 0) 
            // print row number
            printf("%u | ", (i/5) + 1);
        
        // print piece type at ith bit of board
        if ((goats >> i) & 1) {
            printf("G ");
        } else if ((tigers >> i) & 1) {
            printf("T ");
        } else {
            printf("0 ");
        }


        if (((i + 1) % 5) == 0)
            // newline at end of row
            printf("\n");
    }
}

void
print_coord(unsigned char index) 
{
    // convert index to column/row index then convert to ascii
    printf("%c%c", (index % 5) + 65, (index / 5) + 49);
}

void
print_move(Move move) 
{
    const unsigned char move_type = move & 3;

    switch (move_type) {
        case GOAT_MOVE:
            printf("G ");
            print_coord((move >> 2) & 31);
            print_coord((move >> 7) & 31);
            break;
        case TIGER_MOVE:
            printf("T ");
            print_coord((move >> 2) & 31);
            print_coord((move >> 7) & 31);
            break;
        case GOAT_PLACE:
            printf("G ");
            print_coord((move >> 2) & 31);
            break;
        case TIGER_CAPTURE:
            printf("Tx");
            print_coord((move >> 2) & 31);
            print_coord((move >> 7) & 31);
            break;
    }

    printf("\n");
}

void
print_state(State state)
{
    printf("Turn %u, ", state & 1);
    printf("Captured %u, ", (state >> 1) & 7);
    printf("Placed, %u\n", (state >> 4) & 31);
}

void
gen_movelookup(void)
{
    unsigned char i; 
    signed char row, column;
    Board move_mask;

    for (i = 0; i < 25; i++) {
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

void
gen_capturelookup(void)
{
    unsigned char i, u;
    signed char tiger_row, tiger_column, adjacent_row, adjacent_column;
    signed char dest_idx,dest_row, dest_column;
    Board adjacent, capture_mask;

    for (i = 0; i < 25; i++) {
        capture_mask = 0;
        adjacent = movelookup[i];

        for (u = __builtin_ctz(adjacent); u < 25; u = __builtin_ctz(adjacent)) {
            dest_idx = (2 * u) - i;
            tiger_row = i / 5;
            tiger_column = i % 5;
            adjacent_row = u / 5;
            adjacent_column = u % 5;
            dest_row = dest_idx / 5;
            dest_column = dest_idx % 5;
            

           if ((tiger_row - adjacent_row) == (adjacent_row - dest_row) && 
                (tiger_column - adjacent_column) == (adjacent_column - dest_column)) {
                capture_mask |= (1 << dest_idx);
            }

           adjacent ^= 1 << u;
        }

        printf("%u,\n",capture_mask);
        // print_board(capture_mask);
    }
}

unsigned long
perft(Board goats, Board tigers, State state, unsigned char depth)
{
    unsigned long nodes = 0;
    unsigned char i;
    Move moves[25];

    unsigned char num_moves = gen_moves(goats, tigers, state, &moves);

    if (depth == 1)
        return (unsigned long)num_moves;

    for (i = 0; i < num_moves; i++) {
        make_move(&goats, &tigers, &state, moves[i]);
        nodes += perft(goats, tigers, state, depth - 1);
        undo_move(&goats, &tigers, &state, moves[i]);
    }

    return nodes;
}

void
analyze_perft(unsigned char start, unsigned char depth, 
                    unsigned char mode, unsigned char reps) 
{
    Board goats = GOAT_START; 
    Board tigers = TIGER_START;
    State state = STATE_START;

    clock_t begin, end;
    double t_sum = 0.0;


    if (start > depth) {
        printf("Invalid start value: greater than depth\n");
        return;
    }

    printf("Depths %u-%u, Mode %u, Reps %u\n", start, depth, mode, reps);

    switch (mode) {
        case NODES:
            for (unsigned char i = start; i <= depth; i++) {
                printf("%lu\n", perft(goats, tigers, state, i));
            }
            break;
        case TIMED:
            for (unsigned char i = start; i <= depth; i++) {
                for (unsigned char u = 0; u < reps; u++) {
                    begin = clock();
                    perft(goats, tigers, state, i);
                    end = clock();
                    t_sum += (double)(end - begin) / CLOCKS_PER_SEC;
                }
                printf("%lf\n", t_sum / reps);
            }
            break;
        case NODES_TIMED:
            for (unsigned char i = start; i <= depth; i++) {
                for (unsigned char u = 0; u < reps; u++) {
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

void
play_cli(void)
{
    unsigned char input, num_moves, i;
    Board goats = GOAT_START;
    Board tigers = TIGER_START;
    State state = STATE_START;
    Move moves[25];

    while (1) {
        // end game if 5 goats captured 
        if (((state >> 1) & 7) == 5) {
            printf("Tigers win!\n");
            break;
        }

        // end game if tigers have no moves
        num_moves = gen_moves(goats, tigers, state, &moves);
        if ((state & 1) && num_moves == 0) {
            printf("Goats win!\n");
            break;
        }

        // print board
        print_state(state);
        print_move(minimax(goats, tigers, state, 5));
        print_game(goats, tigers); 

        // auto move for tigers
        // if (state & 1) {
        //     make_move(&goats, &tigers, &state, minimax(goats, tigers, state, 5));
        //     continue;
        // }

        // offer list of moves
        for (i = 0; i < num_moves; i++) {
            printf("%u: ", i);
            print_move(moves[i]);
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

        // make move
        make_move(&goats, &tigers, &state, moves[input]);
    }
}
