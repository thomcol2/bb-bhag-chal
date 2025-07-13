#include "test.h"

unsigned long perft(Board goats, Board tigers, State state, unsigned char depth) {
    Move moves[25];
    unsigned long nodes = 0;
    unsigned char num_moves;
    unsigned char i;

    num_moves = gen_moves(goats, tigers, state, &moves);

    if (depth == 1)
        return (unsigned long)num_moves;

    for (i = 0; i < num_moves; i++) {
        make_move(&goats, &tigers, &state, moves[i]);
        nodes += perft(goats, tigers, state, depth - 1);
        undo_move(&goats, &tigers, &state, moves[i]);
    }

    return nodes;
}

void analyze_perft(unsigned char start, unsigned char depth, 
                   unsigned char mode, unsigned char reps) {
    Board goats = GOAT_START; 
    Board tigers = TIGER_START;
    clock_t begin, end;
    double t_sum = 0.0;
    State state = STATE_START;
    unsigned char i, u;

    printf("Depths %u-%u, Mode %u, Reps %u\n", start, depth, mode, reps);

    switch (mode) {
        case NODES:
            for (i = start; i <= depth; i++) {
                printf("%lu\n", perft(goats, tigers, state, i));
            }
            break;
        case TIMED:
            for (i = start; i <= depth; i++) {
                for (u = 0; u < reps; u++) {
                    begin = clock();
                    perft(goats, tigers, state, i);
                    end = clock();
                    t_sum += (double)(end - begin) / CLOCKS_PER_SEC;
                }
                printf("%lf\n", t_sum / reps);
            }
            break;
        case NODES_TIMED:
            for (i = start; i <= depth; i++) {
                for (u = 0; u < reps; u++) {
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
