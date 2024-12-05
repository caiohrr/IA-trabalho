#include <bits/stdc++.h>
#include "z3++.h"

using namespace std;

void readBoard(vector<vector<int>>& board, size_t rows, size_t columns) {
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < columns; j++) {
            cin >> board[i][j];
        }
    }
}

void printBoard(const vector<vector<int>>& board) {
    for (size_t i = 0; i < board.size(); i++) {
        for (size_t j = 0; j < board[i].size(); j++) {
            cout << board[i][j] << " ";
        }
        cout << "\n";
    }
}

int main() {

    size_t rows, columns;
    cin >> rows >> columns;

    vector<vector<int>> board(rows, vector<int>(columns, 0));

    readBoard(board, rows, columns);
    printBoard(board);

    return 0;
}
