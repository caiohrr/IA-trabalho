#include <set>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include "z3++.h"
#include "utils.hpp"
#include "encoding.hpp"

void loneliness(solver &z3_solver, context &z3_context, short x, set<short> &neighbors) {

        // Constrói a cláusula se a célula possui 8 vizinhos, necessário para
        // evitar problemas com células de borda
        if (neighbors.size() != 8) {
                return;
        }
        expr conjunction = z3_context.bool_val(true);

        vector<vector<short>> subsets = generateSubsets(neighbors, 7);
        for (auto &subset : subsets) {
                expr disjunction = setDisjunction(z3_context, subset, false);
                // Junta por conjunção cada disjunção
                conjunction = conjunction && disjunction;
        }
        z3_solver.add(conjunction);
}

void overcrowding(solver &z3_solver, context &z3_context, short x, set<short> &neighbors) {

        if (neighbors.size() < 4) {
                return;
        }

        expr conjunction = z3_context.bool_val(true);

        vector<vector<short>> subsets = generateSubsets(neighbors, 4);
        for (auto &subset : subsets) {
                expr neg_disjunction = setDisjunction(z3_context, subset, true);
                // Junta por conjunção cada disjunção
                conjunction = conjunction && neg_disjunction;
        }
        z3_solver.add(conjunction);
}

void stagnation(solver &z3_solver, context &z3_context, short x, set<short> &neighbors) {

        expr conjunction = z3_context.bool_val(true);

        vector<vector<short>> subsets = generateSubsets(neighbors, 2);
        for (auto &subset : subsets) {
                vector<short> diff_set = {};
                set_difference(neighbors.begin(), neighbors.end(),
                                subset.begin(), subset.end(),
                                inserter(diff_set, diff_set.begin()));

                expr curr_var = z3_context.bool_const(("x_" + to_string(x)).c_str());

                // Disjunção da negação do subconjunto de tamanho 2 atual
                expr neg_disjunction = setDisjunction(z3_context, subset, true);

                // Disjunção da negação da diferença de todos os vizinhos e o subconjunto
                // de tamanho 2 atual
                expr diff_disjunction = setDisjunction(z3_context, diff_set, false);

                // Junta por conjunção cada disjunção
                conjunction = conjunction && (curr_var || neg_disjunction || diff_disjunction);
        }
        z3_solver.add(conjunction);
}

void preservation(solver &z3_solver, context &z3_context, short x, set<short> &neighbors) {

        expr conjunction = z3_context.bool_val(true);

        vector<vector<short>> subsets = generateSubsets(neighbors, 2);
        for (auto &subset : subsets) {
                vector<short> diff_set = {};
                set_difference(neighbors.begin(), neighbors.end(),
                                subset.begin(), subset.end(),
                                inserter(diff_set, diff_set.begin()));

                // Negação da variável atual
                expr curr_var = !z3_context.bool_const(("x_" + to_string(x)).c_str());

                // Disjunção da negação do subconjunto de tamanho 2 atual
                expr neg_disjunction = setDisjunction(z3_context, subset, true);

                // Disjunção da negação da diferença de todos os vizinhos e o subconjunto
                // de tamanho 2 atual
                expr diff_disjunction = setDisjunction(z3_context, diff_set, false);

                // Junta por conjunção cada disjunção
                conjunction = conjunction && (curr_var || neg_disjunction || diff_disjunction);
        }
        z3_solver.add(conjunction);
}

void life(solver &z3_solver, context &z3_context, short x, set<short> &neighbors) {

        expr conjunction = z3_context.bool_val(true);

        vector<vector<short>> subsets = generateSubsets(neighbors, 3);
        for (auto &subset : subsets) {
                vector<short> diff_set = {};
                set_difference(neighbors.begin(), neighbors.end(),
                                subset.begin(), subset.end(),
                                inserter(diff_set, diff_set.begin()));

                // Disjunção das variáveis negadas do subconjunto de tamanho 2 atual
                expr neg_disjunction = setDisjunction(z3_context, subset, true);

                // Disjunção da negação da diferença de todos os vizinhos e o subconjunto
                // de tamanho 2 atual
                expr diff_disjunction = setDisjunction(z3_context, diff_set, false);

                // Junta por conjunção cada disjunção
                conjunction = conjunction && (neg_disjunction || diff_disjunction);
        }
        z3_solver.add(conjunction);
}

