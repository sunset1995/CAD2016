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
#include "sources/simulator.h"

using namespace std;

vector< vector<int> > convert(node n); //convert a gate expression to a cnf expression

bool SAT_accepter(const vector< vector<int> > &cnf, const vector<int> &pat);

bool existed(const vector< vector<int> > &st, const vector<int> &state);

bool SEQ_SAT(Circuit CCT, const vector< vector<int> > &CNF, int N);

Circuit join(const Circuit &a, const Circuit &b);

bool beq(const Circuit &a, const Circuit &b);

/*
to call insert_gate function:
if it's an input or an output, then in1 and in2 are -1
for example, "input 2003" is "0 -1 -1 2003" ; "output 84" is "1 -1 -1 84"
*/

#endif // EQUIVALENCE_H_INCLUDED
