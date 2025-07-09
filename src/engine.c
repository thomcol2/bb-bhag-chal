//
//  engine.c
//
//  REQUIRES:
//      gcc builtin: __builtin_ctz, __builtin_popcount
//      constants.h
//      constants.c
//      moves.h
//      moves.c
//      types.h
//
//  FUNCTIONS:
//      evaluate
//      maximize
//      minimize
//      minimax
//
//  PURPOSES:
//      To define functions that:
//          - evaluate the state of a game
//          - search for the optimal move
//


#include "engine.h"


int 
evaluate(Board goats, Board tigers, State state) 
{
    const Board unoccupied = ~(goats | tigers);
    Board adjacent_goats;
    int eval = 0;
    signed char dest_idx;

    // number of goats captured 
    eval -= ((state >> 1) & 7) * 50;

    // tiger mobility and threats
    for (unsigned char i = __builtin_ctz(tigers); i < 25; i = __builtin_ctz(tigers)) {
        eval -= __builtin_popcount(movelookup[i] & unoccupied);
        
        adjacent_goats = movelookup[i] & goats;
        if (adjacent_goats) {
            for (unsigned char u = __builtin_ctz(adjacent_goats); u < 25; u = __builtin_ctz(adjacent_goats)) {
                dest_idx = (2 * u) - i;

                if ((1 << dest_idx) & capturelookup[i] & unoccupied & 33554431) 
                    eval -= 10;

               adjacent_goats &= adjacent_goats - 1;
            }
        }

        tigers &= tigers - 1;
    }

    // goat piece table
    // tiger piece table

    return eval;
}

int
maximize(Board goats, Board tigers, State state, unsigned char depth)
{
    int best_eval = -10000000;
    int eval;

    // if tigers win return very low eval
    if (((state >> 1) & 7) == 5)
        return -1000000;

    Move moves[25];
    unsigned char num_moves = gen_moves(goats, tigers, state, &moves);

    // if goats win return very high eval
    if (num_moves == 0)
        return 1000000;

    if (depth == 0)
        return evaluate(goats, tigers, state);

    for (unsigned char i = 0; i < num_moves; i++) {
        make_move(&goats, &tigers, &state, moves[i]);
        eval = minimize(goats, tigers, state, depth - 1);
        if (eval > best_eval) {
            best_eval = eval;
        }
        undo_move(&goats, &tigers, &state, moves[i]);
    }

    return best_eval;
}

int
minimize(Board goats, Board tigers, State state, unsigned char depth)
{
    int best_eval = 10000000;
    int eval;

    if ((state >> 1) & 7)
        return -1000000;

    Move moves[25];
    unsigned char num_moves = gen_moves(goats, tigers, state, &moves);

    if (num_moves == 0)
        return 1000000;

    if (depth == 0)
        return evaluate(goats, tigers, state);

    for (unsigned char i = 0; i < num_moves; i++) {
        make_move(&goats, &tigers, &state, moves[i]);
        eval = maximize(goats, tigers, state, depth - 1);
        if (eval < best_eval) {
            best_eval = eval;
        }
        undo_move(&goats, &tigers, &state, moves[i]);
    }

    return best_eval;
}

Move
minimax(Board goats, Board tigers, State state, unsigned char depth)
{
    Move moves[25], best_move;
    int eval, best_eval;
    unsigned char num_moves = gen_moves(goats, tigers, state, &moves);

    if (state & 1) { // minimize if tiger
        best_eval = 10000000;
        for (unsigned char i = 0; i < num_moves; i++) { 
            make_move(&goats, &tigers, &state, moves[i]);
            eval = maximize(goats, tigers, state, depth - 1);
            if (eval < best_eval) {
                best_eval = eval;
                best_move = moves[i];
            }
            undo_move(&goats, &tigers, &state, moves[i]);
        }
    } else { // maximize if goat
        best_eval = -10000000;
        for (unsigned char i = 0; i < num_moves; i++) { 
            make_move(&goats, &tigers, &state, moves[i]);
            eval = minimize(goats, tigers, state, depth - 1);
            if (eval > best_eval) {
                best_eval = eval;
                best_move = moves[i];
            }
            undo_move(&goats, &tigers, &state, moves[i]);
        }
    }

    return best_move;
}
