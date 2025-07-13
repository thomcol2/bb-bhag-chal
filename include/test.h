#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <time.h>

#include "moves.h"
#include "types.h"

enum perf_mode {
    NODES,
    TIMED,
    NODES_TIMED,
};

unsigned long perft(Board goats, Board tigers, State state, unsigned char depth);
void analyze_perft(unsigned char start, unsigned char depth, unsigned char mode, unsigned char reps);
#endif
