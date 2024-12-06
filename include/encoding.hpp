#ifndef ENCODING_HPP
#define ENCODING_HPP

#include <set>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include "z3++.h"
#include "utils.hpp"
#include "encoding.hpp"

using namespace std;
using namespace z3;

// Construção das cláusulas de cada uma das situações:


void loneliness(solver &z3_solver, context &z3_context, short x, set<short> &neighbors);

void overcrowding(solver &z3_solver, context &z3_context, short x, set<short> &neighbors);

void stagnation(solver &z3_solver, context &z3_context, short x, set<short> &neighbors);

void preservation(solver &z3_solver, context &z3_context, short x, set<short> &neighbors);

void life(solver &z3_solver, context &z3_context, short x, set<short> &neighbors);

#endif
