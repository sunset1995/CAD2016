#ifndef SIMULATOR_H
#define SIMULATOR_H


#include <vector>
#include <cstdlib>
#include <utility>
#include "sources/circuit.h"
using namespace std;


pair< vector<bool>, vector<bool> > simulate(const Circuit &cir, const vector<bool> &input, const vector<bool> &dff);


// Get one node signal
bool getSignal(const Circuit &cir, vector<unsigned char> &dp, int nowAt);


#endif
