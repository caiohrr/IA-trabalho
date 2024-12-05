#include <set>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include "z3++.h"

using namespace std;
using namespace z3;

void readBoard(vector<vector<short>> &board, short rows, short columns) {
        for (short i = 0; i < rows; i++) {
                for (short j = 0; j < columns; j++) {
                        cin >> board[i][j];
                }
        }
}

void prshortBoard(const vector<vector<short>> &board, short rows, short columns) {
        for (short i = 0; i < rows; i++) {
                for (short j = 0; j < columns; j++) {
                        cout << (i * rows + j) << " ";
                        //cout << board[i][j] << " ";
                }
                cout << "\n";
        }
}

set<short> generateNeighboursSet(short i, short j, short rows, short columns) {
        set<short> neighbours;


        // Define os offsets para as 8 posições possíveis
        const short offsets[8][2] = {
                {-1, -1}, {-1,  0}, {-1,  1},    // Acima-esquerda, Acima, Acima-direita 
                { 0, -1}, { 0,  1},              // Esquerda, Direita
                { 1, -1}, { 1,  0}, { 1,  1}     // Abaixo-esquerda, Abaixo, Abaixo-direita
        };

        // Itera por cada direção e verifica os vizinhos válidos
        for (int d = 0; d < 8; ++d) {
                short ni = i + offsets[d][0]; 
                short nj = j + offsets[d][1];

                // Verifica se está dentro do tabuleiro
                if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
                        neighbours.insert(ni * rows + nj);  // Adiciona o vizinho no conjunto
                }
        }

        return neighbours;
}

expr setDisjunction(context &z3_context, set<short> &variables) {
        expr_vector var_vector(z3_context);  // Inicializa o vetor com a expressão

        // Itera sobre o conjunto de variáveis
        for (const auto &var : variables) {
                // Create uma variável boolean com o nome "x_{var}"
                var_vector.push_back(z3_context.bool_const(("x_" + to_string(var)).c_str()));
        }

        // Retorna a disjunção de todos os elementos do vetor
        return mk_or(var_vector);
}

//expr conjunctionOfDisjunctions(context &z3_context, vector<vector<short>> &subsets) {
//
//        for (const auto &subset : subsets) {
//                expr disjunction
//        }
//}

vector<vector<short>> generateSubsets(const set<short>& input_set, size_t subset_size) {

        // Se o tamanho do set de entrada for menor que o tamanho esperado dos 
        // subsets, então retorna um conjunto vazio
        if (input_set.size() < subset_size) {
                return {};
        }

        vector<vector<short>> subsets;
        vector<short> setVector(input_set.begin(), input_set.end());  // Converte conjunto pra vetor
        size_t n = setVector.size();

        // Gera todas as combinações para o tamanho desejado
        vector<bool> combination(n);
        fill(combination.begin(), combination.begin() + subset_size, true);

        do {
                vector<short> subset;
                for (size_t i = 0; i < n; ++i) {
                        if (combination[i]) {
                                subset.push_back(setVector[i]);
                        }
                }
                subsets.push_back(subset);
        } while (prev_permutation(combination.begin(), combination.end()));

        return subsets;
}

int main() {

        short rows, columns;
        cin >> rows >> columns;

        vector<vector<short>> board(rows, vector<short>(columns, 0));

        readBoard(board, rows, columns);
        prshortBoard(board, rows, columns);

        context z3_context;

        // Gera as variáveis boolean para cada célula do tabuleiro
        for (short i = 0; i < rows; i ++) {
                for (short j = 0; j < columns; j++) {
                        string curr_var = "x_" + to_string(i * rows + j);
                        cout << curr_var << "\n";
                }
        }

        set<short> neighbours = generateNeighboursSet(1, 1, rows, columns);

        for (const auto &elem : neighbours) {
                cout << elem << " ";
        }
        cout << "\n";

        vector<vector<short>> subsets = generateSubsets(neighbours, 7);
        for (const auto &subset : subsets) {
                for (const auto &elem : subset) {
                        cout << elem << " ";
                }
                cout << "\n";
        }

        return 0;
}
