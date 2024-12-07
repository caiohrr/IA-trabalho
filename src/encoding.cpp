#include <set>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include "z3++.h"
#include "utils.hpp"
#include "encoding.hpp"

void loneliness(z3::solver &z3_solver, z3::context &z3_context, short x, std::set<short> &neighbors) {

        // Constrói a cláusula se a célula possui 8 vizinhos, necessário para
        // evitar problemas com células de borda
        if (neighbors.size() != 8) {
                return;
        }
        z3::expr conjunction = z3_context.bool_val(true);

        std::vector<std::vector<short>> subsets = generateSubsets(neighbors, 7);
        for (auto &subset : subsets) {
                z3::expr disjunction = setDisjunction(z3_context, subset, false);
                // Junta por conjunção cada disjunção
                conjunction = conjunction && disjunction;
        }
        z3_solver.add(conjunction);
}

void overcrowding(z3::solver &z3_solver, z3::context &z3_context, short x, std::set<short> &neighbors) {

        if (neighbors.size() < 4) {
                return;
        }

        z3::expr conjunction = z3_context.bool_val(true);

        std::vector<std::vector<short>> subsets = generateSubsets(neighbors, 4);
        for (auto &subset : subsets) {
                z3::expr neg_disjunction = setDisjunction(z3_context, subset, true);
                // Junta por conjunção cada disjunção
                conjunction = conjunction && neg_disjunction;
        }
        z3_solver.add(conjunction);
}

void stagnation(z3::solver &z3_solver, z3::context &z3_context, short x, std::set<short> &neighbors) {

        z3::expr conjunction = z3_context.bool_val(true);

        std::vector<std::vector<short>> subsets = generateSubsets(neighbors, 2);
        for (auto &subset : subsets) {
                std::vector<short> diff_set = {};
                std::set_difference(neighbors.begin(), neighbors.end(),
                                subset.begin(), subset.end(),
                                std::inserter(diff_set, diff_set.begin()));

                z3::expr curr_var = z3_context.bool_const(("x_" + std::to_string(x)).c_str());

                // Disjunção da negação do subconjunto de tamanho 2 atual
                z3::expr neg_disjunction = setDisjunction(z3_context, subset, true);

                // Disjunção da negação da diferença de todos os vizinhos e o subconjunto
                // de tamanho 2 atual
                z3::expr diff_disjunction = setDisjunction(z3_context, diff_set, false);

                // Junta por conjunção cada disjunção
                conjunction = conjunction && (curr_var || neg_disjunction || diff_disjunction);
        }
        z3_solver.add(conjunction);
}

void preservation(z3::solver &z3_solver, z3::context &z3_context, short x, std::set<short> &neighbors) {

        z3::expr conjunction = z3_context.bool_val(true);

        std::vector<std::vector<short>> subsets = generateSubsets(neighbors, 2);
        for (auto &subset : subsets) {
                std::vector<short> diff_set = {};
                std::set_difference(neighbors.begin(), neighbors.end(),
                                subset.begin(), subset.end(),
                                std::inserter(diff_set, diff_set.begin()));

                // Negação da variável atual
                z3::expr curr_var = !z3_context.bool_const(("x_" + std::to_string(x)).c_str());

                // Disjunção da negação do subconjunto de tamanho 2 atual
                z3::expr neg_disjunction = setDisjunction(z3_context, subset, true);

                // Disjunção da negação da diferença de todos os vizinhos e o subconjunto
                // de tamanho 2 atual
                z3::expr diff_disjunction = setDisjunction(z3_context, diff_set, false);

                // Junta por conjunção cada disjunção
                conjunction = conjunction && (curr_var || neg_disjunction || diff_disjunction);
        }
        z3_solver.add(conjunction);
}

void life(z3::solver &z3_solver, z3::context &z3_context, short x, std::set<short> &neighbors) {

        z3::expr conjunction = z3_context.bool_val(true);

        std::vector<std::vector<short>> subsets = generateSubsets(neighbors, 3);
        for (auto &subset : subsets) {
                std::vector<short> diff_set = {};
                std::set_difference(neighbors.begin(), neighbors.end(),
                                subset.begin(), subset.end(),
                                std::inserter(diff_set, diff_set.begin()));

                // Disjunção das variáveis negadas do subconjunto de tamanho 2 atual
                z3::expr neg_disjunction = setDisjunction(z3_context, subset, true);

                // Disjunção da negação da diferença de todos os vizinhos e o subconjunto
                // de tamanho 2 atual
                z3::expr diff_disjunction = setDisjunction(z3_context, diff_set, false);

                // Junta por conjunção cada disjunção
                conjunction = conjunction && (neg_disjunction || diff_disjunction);
        }
        z3_solver.add(conjunction);
}
