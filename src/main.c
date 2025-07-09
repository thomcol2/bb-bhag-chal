#include <stdio.h>

#include "engine.h"
#include "moves.h"
#include "utils.h"

int main() {
    // TODO
    // - Improve evaluation function
    // - Implement analysis function for minimax
    // - Implement alpha-beta pruning
    // - Develop TUI/GUI for game
    // - Implement draw rule

    analyze_perft(1, 7, NODES_TIMED, 20);
    // analyze_perft(8, 8, TIMED, 5);
    // analyze_perft(9, 9, TIMED, 1);

    // play_cli();

    return 0;
}
