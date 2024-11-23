#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

int iteration = 1;

void generatePossibilities(int **board, int rows, int columns, int cell, int state) {

        if (cell >= rows * columns) {
                //printBoard(board, rows, columns);
                return;
        }
        
        int r, c;
        iteration++;

        r = cell / rows;
        c = cell % rows;
        //printf("%d %d\n", r, c);
        board[r][c] = state;

        generatePossibilities(board, rows, columns, cell + 1, 0);
        generatePossibilities(board, rows, columns, cell + 1, 1);
}

int main() {

        int rows, columns;
        scanf("%d %d", &rows, &columns);

        int ** board = allocBoard(rows, columns);

        readBoard(board, rows, columns);
        //printBoard(board, rows, columns);

        generatePossibilities(board, rows, columns, 0, 0);

        printf("%d\n", iteration);

        return 0;
}
