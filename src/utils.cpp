#include <set>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include "z3++.h"
#include "utils.hpp"

void readBoard(vector<vector<short>> &board, short rows, short columns) {
        for (short i = 0; i < rows; i++) {
                for (short j = 0; j < columns; j++) {
                        cin >> board[i][j];
                }
        }
}

void printShortBoard(const vector<vector<short>> &board, short rows, short columns) {
        for (short i = 0; i < rows; i++) {
                for (short j = 0; j < columns; j++) {
                        cout << board[i][j] << " ";
                }
                cout << "\n";
        }
}

// Função para imprimir o tabuleiro reconstruído com base nas valorações do SAT solver
void printReconstructedBoard(const vector<expr>& variables, model& z3_model, short rows, short columns) {
        size_t idx = 0;
        for (short i = 0; i < rows; i++) {
                for (short j = 0; j < columns; j++) {
                        // Pega o valor da variável 
                        expr value = z3_model.eval(variables[idx], true);  
                        cout << (value.bool_value() == Z3_L_TRUE ? 1 : 0) << " ";  // 1 para verdadeiro, 0 para falso 
                        idx++;
                }
                cout << "\n";
        }
}

set<short> generateNeighborsSet(short i, short j, short rows, short columns) {

        set<short> neighbors;

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
                        neighbors.insert(ni * rows + nj);  // Adiciona o vizinho no conjunto
                }
        }

        return neighbors;
}

// Cria a disjunção para um conjunto de variáveis
// Se negate for true, cada variável será negada
expr setDisjunction(context &z3_context, vector<short> &variables, bool negate) {
        expr_vector var_vector(z3_context);  // Inicializa o vetor com a expressão

        // Itera sobre o conjunto de variáveis
        for (const auto &var : variables) {
                // Gera uma variável boolean com o nome "x_{var}"
                expr curr_var = z3_context.bool_const(("x_" + to_string(var)).c_str());

                if (negate) {
                        curr_var = !curr_var;
                }
                var_vector.push_back(curr_var);
        }

        // Retorna a disjunção de todos os elementos do vetor
        return mk_or(var_vector);
}


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

