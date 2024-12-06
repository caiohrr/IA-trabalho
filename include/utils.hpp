#ifndef UTILS_HPP
#define UTILS_HPP

#include <set>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include "z3++.h"

using namespace std;
using namespace z3;

void readBoard(vector<vector<short>> &board, short rows, short columns);

void printShortBoard(const vector<vector<short>> &board, short rows, short columns);

// Função para imprimir o tabuleiro reconstruído com base nas valorações do SAT solver
void printReconstructedBoard(const vector<expr>& variables, model& z3_model, short rows, short columns);

set<short> generateNeighborsSet(short i, short j, short rows, short columns);

// Cria a disjunção para um conjunto de variáveis
// Se negate for true, cada variável será negada
expr setDisjunction(context &z3_context, vector<short> &variables, bool negate);

vector<vector<short>> generateSubsets(const set<short>& input_set, size_t subset_size);

#endif
