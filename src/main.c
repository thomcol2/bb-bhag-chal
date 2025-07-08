#include <stdio.h>

#include "constants.h"
#include "moves.h"
#include "types.h"
#include "utils.h"

int main() {
    // TODO
    // - Improve documentation in source and readme 
    // - Develop algorithm for finding the best move
    // - - Static evaluation function
    // - - Min-max searching
    // - - Alpha-beta pruning
    // - Develop TUI/GUI for game

    // analyze_performance(1, 7, TIMED, 20);
    // analyze_performance(8, 8, TIMED, 5);
    // analyze_performance(9, 9, TIMED, 1);

    play_cli();

    return 0;
}
