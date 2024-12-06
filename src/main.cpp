#include <set>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <chrono>
#include "z3++.h"
#include "utils.hpp"
#include "encoding.hpp"

using namespace std::chrono;
using namespace std;
using namespace z3;

int main() {

        auto start_time = high_resolution_clock::now();
        const int time_limit = 300; // Tempo limite, 5 minutos

        short rows, columns;
        cin >> rows >> columns;

        vector<vector<short>> board(rows, vector<short>(columns, 0));
        readBoard(board, rows, columns);

        context z3_context;
        solver z3_solver(z3_context);

        z3_solver.set("timeout", static_cast<unsigned>(150000)); // 150 segundos de máximo para um check do z3

        expr num_true = z3_context.int_const("num_true");
        z3_solver.add(num_true >= 0);

        vector<expr> variables;

        // Itera sobre o tabuleiro e adiciona uma variável para cada célula,
        // além das cláusulas próprias do game of life dependendo do estado

        for (short i = 0; i < rows; i++) {
                for (short j = 0; j < columns; j++) {
                        short x = i * rows + j;
                        expr curr_var = z3_context.bool_const(("x_" + to_string(x)).c_str());

                        variables.push_back(curr_var);

                        set<short> x_neighbors = generateNeighborsSet(i, j, rows, columns);

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
        expr_vector sum_exprs(z3_context);
        for (const auto &var : variables) {
                sum_exprs.push_back(z3::ite(var, z3_context.int_val(1), z3_context.int_val(0)));
        }
        z3_solver.add(num_true == sum(sum_exprs));

        int min_true_clauses = INT_MAX;
        vector<vector<short>> best_board(rows, vector<short>(columns, 0));

        while (true) {
                auto current_time = high_resolution_clock::now();
                auto elapsed_time = duration_cast<seconds>(current_time - start_time).count();

                if (elapsed_time >= time_limit) {
                        // cout << "Tempo limite atingido. imprimindo o melhor tabuleiro encontrado.\n";
                        break;
                }

                auto current_iteration_start = high_resolution_clock::now();
                if (z3_solver.check() != sat) {
                        // cout << "Não há mais soluções a serem encontradas.\n";
                        break;
                }
                auto current_iteration_stop = high_resolution_clock::now();
                auto current_iteration_time = duration_cast<seconds>(current_iteration_stop - current_iteration_start).count();

                model z3_model = z3_solver.get_model();
                int true_clauses = 0;

                for (const auto &var : variables) {
                        if (z3_model.eval(var, true).bool_value() == Z3_L_TRUE) {
                                true_clauses++;
                        }
                }

                if (true_clauses < min_true_clauses) {
                        min_true_clauses = true_clauses;

                        // Pode ser habilitado para ver o tabuleiro a cada valoração nova:
                        // cout << "\nTrue vars = " << true_clauses << "\n";
                        // printShortBoard(best_board, rows, columns);

                        size_t idx = 0;
                        for (short i = 0; i < rows; i++) {
                                for (short j = 0; j < columns; j++) {
                                        expr value = z3_model.eval(variables[idx], true);
                                        best_board[i][j] = (value.bool_value() == Z3_L_TRUE ? 1 : 0);
                                        idx++;
                                }
                        }
                }

                z3_solver.add(num_true < min_true_clauses);

                // Estimativa de que a próxima iteração irá ultrapassar o limite de tempo
                // Margem que estima que a proxima iteração será 20% mais lenta do que a atual

                auto time_estimate = (elapsed_time + current_iteration_time) * 1.2;

                // Informações sobre o tempo:
                //cout << "Time estimate for next iteration: " << time_estimate << "\n";
                //cout << "The next iteration will end at " << time_estimate + duration_cast<seconds>(current_time - start_time).count() << " s of execution time\n";
                //cout << "Time limit is " << time_limit << "\n";

                if (time_estimate + duration_cast<seconds>(current_time - start_time).count() > time_limit) {
                        break;
                }
        }

        printShortBoard(best_board, rows, columns);

        // Indicação do número de células vivas:
        //cout << "Best board found with " << min_true_clauses << " true clauses." << endl;

        return 0;
}
