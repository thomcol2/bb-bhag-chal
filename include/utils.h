#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <time.h>

#include "moves.h"
#include "types.h"

// print functions
void print_board(BOARD board);
void print_game(BOARD goats, BOARD tigers);
void print_indices(void);
void print_coord(u8 index);
void print_move(MOVE move);
void print_state(STATE state);

//
void play_cli(void);

// performance test functions
u64 perft(BOARD goats, BOARD tigers, STATE state, u8 depth);
void analyze_performance(u8 start, u8 depth, u8 mode, u8 reps);

// lookup generation functions
void gen_movelookup(void);
void gen_capturelookup(void);
void gen_coordinatelookup(void);

// conversion functions
u8 string_to_index(const char coord[3]);
#endif
