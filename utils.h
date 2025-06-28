#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include "types.h"
#include "constants.h"

void print_board(BOARD board) {
    printf("    A B C D E\n");

    for (u8 i = 0; i < 25; i++) {
        if ((i % 5) == 0) {
            printf("%u | ", (i/5) + 1);
        }
        
        printf("%u ", (board >> i) & 1);

        if (((i + 1) % 5) == 0)
            printf("\n");
    }
}

void print_game(BOARD goats, BOARD tigers) {
    // TODO
    //  - Add game state display
    printf("    A B C D E\n");

    for (u8 i = 0; i < 25; i++) {
        if ((i % 5) == 0) {
            printf("%u | ", (i/5) + 1);
        }
        
        if ((goats >> i) & 1) {
            printf("G ");
        } else if ((tigers >> i) & 1) {
            printf("T ");
        } else {
            printf("0 ");
        }


        if (((i + 1) % 5) == 0)
            printf("\n");
    }
}

void print_indices(void) {
    printf("    A  B  C  D  E\n");

    for (u8 i = 0; i < 25; i++) {
        if ((i % 5) == 0) {
            printf("%u | ", (i/5) + 1);
        }
        
        if (i < 10) {
            printf("%u  ", i);
        } else {
            printf("%u ", i);
        }

        if (((i + 1) % 5) == 0)
            printf("\n");
    }
}

void print_coord(u8 index) {
    u8 row = (index / 5) + 49; // convert row to ascii [12345]
    u8 column = (index % 5) + 65; // convert row to ascii [ABCDE]

    printf("%c%c", column, row);
}

void print_move(MOVE move) {
    u8 move_type = move & 3;

    switch (move_type) {
        case GOAT_MOVE:
            printf("G ");
            break;
        case TIGER_MOVE:
            printf("T ");
            break;
        case GOAT_PLACE:
            printf("G ");
            break;
        case TIGER_CAPTURE:
            printf("Tx");
            break;
    }

    if (move_type == GOAT_PLACE) {
        print_coord((move >> 2) & 31);
        printf("\n");
    } else {
        print_coord((move >> 2) & 31);
        print_coord((move >> 7) & 31);
        printf("\n");
    }
}

u8 string_to_index(const char coord[3]) {
    u8 row = coord[1] % 49; // convert ascii to integer value (0-4)
    u8 column = coord[0] % 65; // convert ascii to integer value (0-4)

    // calculate index 
    u8 index = (row * 5 ) + column;
    return index;
}

void gen_movelookup(void) {
    s8 row, column;
    BOARD board;

    for (u8 i = 0; i < 25; i++) {
        board = 0;
        row = i / 5;
        column = i % 5;

        // calculate adjacent orthogonal positions
        if ( (column+1) <= 4)
            board |= 1 << ((row * 5) + (column+1));
        if ( (column-1) >= 0)
            board |= 1 << ((row * 5) + (column-1));
        if ( (row+1) <= 4)
            board |= 1 << (((row+1) * 5) + column);
        if ( (row-1) >= 0)
            board |= 1 << (((row-1) * 5) + column);

        // only calculate diagonals if the index is even
        if ( (i % 2) == 0) {
            if ( (column+1) <= 4 && (row+1) <= 4)
                board |= 1 << (((row+1) * 5) + (column+1));
            if ( (column+1) <= 4 && (row-1) >= 0)
                board |= 1 << (((row-1) * 5) + (column+1));
            if ( (column-1) >= 0 && (row+1) <= 4)
                board |= 1 << (((row+1) * 5) + (column-1));
            if ( (column-1) >= 0 && (row-1) >= 0)
                board |= 1 << (((row-1) * 5) + (column-1));
        }

        printf("%u,\n", board);
    }
}
#endif
