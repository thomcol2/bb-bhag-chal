#include <stdio.h>

#include "engine.h"
#include "test.h"
#include "utils.h"

void play_cli(void) {
    Board tigers = TIGER_START;
    Board goats = GOAT_START;
    Move moves[25];
    State state = STATE_START;
    unsigned char input, num_moves;
    unsigned char i;

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

        print_state(state);
        // printf("%i\n", evaluate(goats, tigers, state));
        // minimax(goats, tigers, state, 5);
        print_game(goats, tigers); 


        // offer list of moves
        for (i = 0; i < num_moves; i++) {
            printf("%u: ", i);
            print_move(moves[i]);
        }

        if (state & 1) {
            make_move(&goats, &tigers, &state, minimax(goats, tigers, state, 5));
            continue;
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

        make_move(&goats, &tigers, &state, moves[input]);
    }
}

int main() {
    // TODO
    // - Implement alpha-beta pruning
    // - Implement repetition draw rule
    // - Develop TUI/GUI for game

    analyze_perft(1, 7, NODES_TIMED, 20);
    // analyze_perft(8, 8, TIMED, 5);
    // analyze_perft(9, 9, TIMED, 1);

    // play_cli();

    return 0;
}
