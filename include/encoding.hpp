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

// Construção das cláusulas de cada uma das situações:

void loneliness(z3::solver &z3_solver, z3::context &z3_context, short x, std::set<short> &neighbors);

void overcrowding(z3::solver &z3_solver, z3::context &z3_context, short x, std::set<short> &neighbors);

void stagnation(z3::solver &z3_solver, z3::context &z3_context, short x, std::set<short> &neighbors);

void preservation(z3::solver &z3_solver, z3::context &z3_context, short x, std::set<short> &neighbors);

void life(z3::solver &z3_solver, z3::context &z3_context, short x, std::set<short> &neighbors);

#endif

