#include <set>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <chrono>
#include <thread>
#include "z3++.h"
#include "utils.hpp"
#include "encoding.hpp"

int main() {
        auto start_time = std::chrono::high_resolution_clock::now();
        const int time_limit = 300; // Tempo limite, 5 minutos

        short rows, columns;
        std::cin >> rows >> columns;

        std::vector<std::vector<short>> board(rows, std::vector<short>(columns, 0));
        readBoard(board, rows, columns);


        z3::context z3_context;
        z3::solver z3_solver(z3_context);

        z3::params z3_p(z3_context);
        z3_p.set("threads", std::thread::hardware_concurrency());
        z3_solver.set(z3_p);

        unsigned threads = std::thread::hardware_concurrency();
        std::cout << "Setting Z3 to use " << threads << " threads." << std::endl;

        z3_solver.set("timeout", static_cast<unsigned>(150000)); // 150 segundos de máximo para um check do z3

        z3::expr num_true = z3_context.int_const("num_true");
        z3_solver.add(num_true >= 0);

        std::vector<z3::expr> variables;

        // Itera sobre o tabuleiro e adiciona uma variável para cada célula,
        // além das cláusulas próprias do game of life dependendo do estado
        for (short i = 0; i < rows; i++) {
                for (short j = 0; j < columns; j++) {
                        short x = i * rows + j;
                        z3::expr curr_var = z3_context.bool_const(("x_" + std::to_string(x)).c_str());

                        variables.push_back(curr_var);

                        std::set<short> x_neighbors = generateNeighborsSet(i, j, rows, columns);

                        if (board[i][j] == 1) {
                                loneliness(z3_solver, z3_context, x, x_neighbors);
                                stagnation(z3_solver, z3_context, x, x_neighbors);
                                overcrowding(z3_solver, z3_context, x, x_neighbors);
                        } else {
                                preservation(z3_solver, z3_context, x, x_neighbors);
                                life(z3_solver, z3_context, x, x_neighbors);
                        }
                }
        }

        // Restrição para contar o número de células vivas
        // Necessário para buscar o tabuleiro com menor número de células vivas
        z3::expr_vector sum_exprs(z3_context);
        for (const auto &var : variables) {
                sum_exprs.push_back(z3::ite(var, z3_context.int_val(1), z3_context.int_val(0)));
        }
        z3_solver.add(num_true == sum(sum_exprs));

        int min_true_clauses = INT_MAX;
        std::vector<std::vector<short>> best_board(rows, std::vector<short>(columns, 0));

        while (true) {
                auto current_time = std::chrono::high_resolution_clock::now();
                auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();

                if (elapsed_time >= time_limit) {
                        break;
                }

                auto current_iteration_start = std::chrono::high_resolution_clock::now();
                if (z3_solver.check() != z3::sat) {
                        break;
                }
                auto current_iteration_stop = std::chrono::high_resolution_clock::now();
                auto current_iteration_time = std::chrono::duration_cast<std::chrono::seconds>(current_iteration_stop - current_iteration_start).count();

                z3::model z3_model = z3_solver.get_model();
                int true_clauses = 0;

                for (const auto &var : variables) {
                        if (z3_model.eval(var, true).bool_value() == Z3_L_TRUE) {
                                true_clauses++;
                        }
                }

                if (true_clauses < min_true_clauses) {
                        min_true_clauses = true_clauses;

                        size_t idx = 0;
                        for (short i = 0; i < rows; i++) {
                                for (short j = 0; j < columns; j++) {
                                        z3::expr value = z3_model.eval(variables[idx], true);
                                        best_board[i][j] = (value.bool_value() == Z3_L_TRUE ? 1 : 0);
                                        idx++;
                                }
                        }
                }

                z3_solver.add(num_true < min_true_clauses);

                auto time_estimate = (elapsed_time + current_iteration_time) * 1.2;

                if (time_estimate + std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count() > time_limit) {
                        break;
                }
        }

        printShortBoard(best_board, rows, columns);

        return 0;
}
