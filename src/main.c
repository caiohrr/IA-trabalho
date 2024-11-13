#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

int main() {

        int rows, columns;
        scanf("%d %d", &rows, &columns);

        int ** board = allocBoard(rows, columns);
        readBoard(board, rows, columns);
        printBoard(board, rows, columns);

        return 0;
}
