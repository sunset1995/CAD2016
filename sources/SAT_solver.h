#ifndef __SAT_SOLVER_H
#define __SAT_SOLVER_H

#include <vector>
#include <errno.h>
#include <zlib.h>
using namespace std;

#include "sources/minisat/utils/System.h"
#include "sources/minisat/utils/ParseUtils.h"
#include "sources/minisat/utils/Options.h"
#include "sources/minisat/core/SolverTypes.h"
#include "sources/minisat/core/Solver.h"
using namespace Minisat;

/*
    Given boolean formular in CNF and number of distinct variable.
    Return whether satisfy or not.

    Each clause store in vector<int>.
    All variable id from 1~N, negative means invert.
*/
bool SAT_solver(vector< vector<int> >, int);

#endif
