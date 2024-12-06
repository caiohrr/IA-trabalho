#include <set>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <chrono>
#include "z3++.h"

using namespace std::chrono;
using namespace std;
using namespace z3;

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
                        expr value = z3_model.eval(variables[idx], true);  // Get the value of the variable from the model
                        cout << (value.bool_value() == Z3_L_TRUE ? 1 : 0) << " ";  // 1 if true, 0 if false
                        idx++;
                }
                cout << endl;
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


        //        size_t n1 = setVector.size();
        //        // Calculate total number of subsets: binomial coefficient C(n, subset_size)
        //        size_t total_subsets = 1;
        //        for (size_t i = 0; i < subset_size; ++i) {
        //                total_subsets *= (n1 - i);
        //                total_subsets /= (i + 1);
        //        }
        //
        //        cout << "Generating " << total_subsets << " subsets of size " << subset_size 
        //                << " from a set of size " << n1 << "." << endl;
        //
        //


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
        //cout << "Loneliness clause for " << "x_" << x << ":" << conjunction << "\n";
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
        //cout << "Overcrowding clause for " << "x_" << x << ":" << conjunction << "\n";
}

void stagnation(solver &z3_solver, context &z3_context, short x, set<short> &neighbors) {

        expr conjunction = z3_context.bool_val(true);

        vector<vector<short>> subsets = generateSubsets(neighbors, 2);
        for (auto &subset : subsets) {
                vector<short> diff_set = {};
                set_difference(neighbors.begin(), neighbors.end(),
                                subset.begin(), subset.end(),
                                inserter(diff_set, diff_set.begin()));

                //cout << "set diff:\n";
                //for (auto const &elem : diff_set)
                //        cout << elem << " ";
                //cout << "\n";

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
        //cout << "Stagnation clause for " << "x_" << x << ":" << conjunction << "\n";
}

void preservation(solver &z3_solver, context &z3_context, short x, set<short> &neighbors) {

        expr conjunction = z3_context.bool_val(true);

        vector<vector<short>> subsets = generateSubsets(neighbors, 2);
        for (auto &subset : subsets) {
                vector<short> diff_set = {};
                set_difference(neighbors.begin(), neighbors.end(),
                                subset.begin(), subset.end(),
                                inserter(diff_set, diff_set.begin()));

                //cout << "set diff:\n";
                //for (auto const &elem : diff_set)
                //        cout << elem << " ";
                //cout << "\n";

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
        //cout << "Preservation clause for " << "x_" << x << ":" << conjunction << "\n";
}

void life(solver &z3_solver, context &z3_context, short x, set<short> &neighbors) {

        expr conjunction = z3_context.bool_val(true);

        vector<vector<short>> subsets = generateSubsets(neighbors, 3);
        for (auto &subset : subsets) {
                vector<short> diff_set = {};
                set_difference(neighbors.begin(), neighbors.end(),
                                subset.begin(), subset.end(),
                                inserter(diff_set, diff_set.begin()));

                //cout << "set diff:\n";
                //for (auto const &elem : diff_set)
                //        cout << elem << " ";
                //cout << "\n";

                // Disjunção das variáveis negadas do subconjunto de tamanho 2 atual
                expr neg_disjunction = setDisjunction(z3_context, subset, true);

                // Disjunção da negação da diferença de todos os vizinhos e o subconjunto
                // de tamanho 2 atual
                expr diff_disjunction = setDisjunction(z3_context, diff_set, false);

                // Junta por conjunção cada disjunção
                conjunction = conjunction && (neg_disjunction || diff_disjunction);
        }
        z3_solver.add(conjunction);
        //cout << "Life clause for " << "x_" << x << ":" << conjunction << "\n";
}

int main() {

        auto start_time = high_resolution_clock::now();
        const int time_limit = 300; // Tempo limite

        short rows, columns;
        cin >> rows >> columns;

        vector<vector<short>> board(rows, vector<short>(columns, 0));
        readBoard(board, rows, columns);

        context z3_context;
        solver z3_solver(z3_context);

        z3_solver.set("timeout", static_cast<unsigned>(150000)); // 150 seconds for individual checks (optional, redundant with loop timeout)

        expr num_true = z3_context.int_const("num_true");
        z3_solver.add(num_true >= 0);

        vector<expr> variables;
        for (short i = 0; i < rows; i++) {
                for (short j = 0; j < columns; j++) {
                        expr curr_var = z3_context.bool_const(("x_" + to_string(i * rows + j)).c_str());
                        variables.push_back(curr_var);
                }
        }

        for (short i = 0; i < rows; i++) {
                for (short j = 0; j < columns; j++) {
                        short x = i * rows + j;
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


                cout << elapsed_time << "\n";

                if (elapsed_time >= time_limit) {
                        cout << "Time limit reached. Printing the best board found.\n";
                        break;
                }
                
                auto current_iteration_start = high_resolution_clock::now();
                if (z3_solver.check() != sat) {
                        cout << "No more solutions found.\n";
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
                        cout << "\nTrue vars = " << true_clauses << "\n";
                        printShortBoard(best_board, rows, columns);

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
                //
                auto time_estimate = (elapsed_time + current_iteration_time) * 1.2;

                cout << "Time estimate for next iteration: " << time_estimate << "\n";
                cout << "The next iteration will end at " << time_estimate + duration_cast<seconds>(current_time - start_time).count() << " s of execution time\n";
                cout << "Time limit is " << time_limit << "\n";
                if (time_estimate + duration_cast<seconds>(current_time - start_time).count() > time_limit) {
                        break;
                }
        }

        printShortBoard(best_board, rows, columns);
        cout << "Best board found with " << min_true_clauses << " true clauses." << endl;

        return 0;
}
