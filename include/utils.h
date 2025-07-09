#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <time.h>

#include "engine.h"
#include "moves.h"
#include "types.h"

enum perf_mode {
    NODES,
    TIMED,
    NODES_TIMED,
};

// print functions
void print_board(Board board);
void print_game(Board goats, Board tigers);
void print_coord(unsigned char index);
void print_move(Move move);
void print_state(State state);

//
void play_cli(void);

// performance test functions
unsigned long perft(Board goats, Board tigers, State state, unsigned char depth);
void analyze_perft(unsigned char start, unsigned char depth, unsigned char mode, unsigned char reps);

// lookup generation functions
void gen_movelookup(void);
void gen_capturelookup(void);
void gen_coordinatelookup(void);
#endif
