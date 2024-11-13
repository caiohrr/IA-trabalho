#include "utils.h"

int *allocIntArray(int size) {
        int *arr = malloc(sizeof(int) * size);
        if (!arr) {
                return NULL;
        }
        return arr;
}
int **allocBoard(int rows, int columns) {
        int **board = malloc(sizeof(int *) * rows);
        if (!board) {
                return NULL;
        }

        for (int i = 0; i < rows; i++) {
                board[i] = allocIntArray(columns);
        }

        return board;
}

void printBoard(int **board, int rows, int columns) {

        for (int i = 0; i < rows; i++) {
                for (int j = 0; j < columns; j++) {
                        printf("%d ", board[i][j]);
                }
                printf("\n");
        }
}

void readBoard(int **board, int rows, int columns) {

        for (int i = 0; i < rows; i++) {
                for (int j = 0; j < columns; j++) {
                        scanf("%d", &board[i][j]);
                }
        }
}
