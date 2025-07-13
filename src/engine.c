//
//  engine.c
//
//  REQUIRES:
//      gcc builtin: __builtin_ctz, __builtin_popcount
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

int evaluate(Board goats, Board tigers, State state) {
    int eval = 0;
    unsigned char captured;

    captured = (state >> 1) & 7;
    eval -= captured * 10;

    return eval;
}

int maximize(Board goats, Board tigers, State state, unsigned char depth) {
    int best_eval = -10000000;
    int eval;
    Move moves[25];
    unsigned char num_moves, captured;
    unsigned char i;

    captured = (state >> 1) & 7;
    if (captured == 5)
        return -1000000;

    num_moves = gen_moves(goats, tigers, state, &moves);
    if (num_moves == 0)
        return 1000000;

    if (depth == 0)
        return evaluate(goats, tigers, state);

    for (i = 0; i < num_moves; i++) {
        make_move(&goats, &tigers, &state, moves[i]);
        eval = minimize(goats, tigers, state, depth - 1);
        if (eval > best_eval) {
            best_eval = eval;
        }
        undo_move(&goats, &tigers, &state, moves[i]);
    }

    return best_eval;
}

int minimize(Board goats, Board tigers, State state, unsigned char depth) {
    int best_eval = 10000000;
    int eval;
    Move moves[25];
    unsigned char num_moves, captured;
    unsigned char i;

    captured = (state >> 1) & 7;
    if (captured == 5)
        return -1000000;

    num_moves = gen_moves(goats, tigers, state, &moves);
    if (num_moves == 0)
        return 1000000;

    if (depth == 0)
        return evaluate(goats, tigers, state);

    for (i = 0; i < num_moves; i++) {
        make_move(&goats, &tigers, &state, moves[i]);
        eval = maximize(goats, tigers, state, depth - 1);
        if (eval < best_eval) {
            best_eval = eval;
        }
        undo_move(&goats, &tigers, &state, moves[i]);
    }

    return best_eval;
}

Move minimax(Board goats, Board tigers, State state, unsigned char depth) {
    int eval, best_eval;
    Move moves[25], best_move;
    unsigned char num_moves, turn;
    unsigned char i;

    num_moves = gen_moves(goats, tigers, state, &moves);
    turn = state & 1;

    switch (turn) {
        case TIGER_TURN:
            best_eval = 10000000;
            for (i = 0; i < num_moves; i++) { 
                make_move(&goats, &tigers, &state, moves[i]);
                eval = maximize(goats, tigers, state, depth - 1);
                if (eval < best_eval) {
                    best_eval = eval;
                    best_move = moves[i];
                }
                undo_move(&goats, &tigers, &state, moves[i]);
            }
            break;
        case GOAT_TURN:
            best_eval = -10000000;
            for (i = 0; i < num_moves; i++) { 
                make_move(&goats, &tigers, &state, moves[i]);
                eval = minimize(goats, tigers, state, depth - 1);
                if (eval > best_eval) {
                    best_eval = eval;
                    best_move = moves[i];
                }
                undo_move(&goats, &tigers, &state, moves[i]);
            }
            break;
    }

    return best_move;
}
