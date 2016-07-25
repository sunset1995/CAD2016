/*
Circuit graph transform to CNF
Circuit equvalence checking
*/
#ifndef EQUIVALENCE_H_INCLUDED
#define EQUIVALENCE_H_INCLUDED

#include <cstdio>
#include <cstdlib>
#include <unordered_set>
#include <vector>
#include "sources/circuit.h"
#include "sources/SAT_solver.h"

using namespace std;

vector< vector<int> > convert(node n); //convert a gate expression to a cnf expression

bool SEQ_SAT(Circuit CCT, vector< vector<int> > CNF, int N);

Circuit join(const Circuit &a, const Circuit &b);

bool beq(const Circuit &a, const Circuit &b);

/*
to call insert_gate function:
if it's an input or an output, then in1 and in2 are -1
for example, "input 2003" is "0 -1 -1 2003" ; "output 84" is "1 -1 -1 84"
*/

#endif // EQUIVALENCE_H_INCLUDED
