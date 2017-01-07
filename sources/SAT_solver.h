#ifndef __SAT_SOLVER_H
#define __SAT_SOLVER_H

#include <ctime>
#include <vector>
#include <algorithm>
#include <errno.h>
#include <zlib.h>
#include "sources/yasat/solver.h"
using namespace std;

/*
    Given boolean formular in CNF and number of distinct variable.
    Return whether satisfy or not.

    Each clause store in vector<int>.
    All variable id from 1~N, negative means invert.
*/
vector<int> SAT_solver(vector< vector<int> >, int);

#endif
