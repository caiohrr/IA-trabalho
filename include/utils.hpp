#ifndef UTILS_HPP
#define UTILS_HPP

#include <set>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include "z3++.h"

void readBoard(std::vector<std::vector<short>> &board, short rows, short columns);

void printShortBoard(const std::vector<std::vector<short>> &board, short rows, short columns);

// Função para imprimir o tabuleiro reconstruído com base nas valorações do SAT solver
void printReconstructedBoard(const std::vector<z3::expr>& variables, z3::model& z3_model, short rows, short columns);

std::set<short> generateNeighborsSet(short i, short j, short rows, short columns);

// Cria a disjunção para um conjunto de variáveis
// Se negate for true, cada variável será negada
z3::expr setDisjunction(z3::context &z3_context, std::vector<short> &variables, bool negate);

std::vector<std::vector<short>> generateSubsets(const std::set<short>& input_set, size_t subset_size);

#endif
