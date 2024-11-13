#include <stdio.h>
#include <stdlib.h>

int *allocIntArray(int size);

int **allocBoard(int rows, int columns);

void printBoard(int **board, int rows, int columns);

void readBoard(int **board, int rows, int columns);
