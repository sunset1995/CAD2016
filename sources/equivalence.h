/*
Circuit graph transform to CNF
Circuit equvalence checking
*/
#ifndef EQUIVALENCE_H_INCLUDED
#define EQUIVALENCE_H_INCLUDED

#include <cstdio>
#include <cstdlib>
#include <map>
#include <vector>
#include "sources/circuit.h"

using namespace std;

vector< vector<int> > convert(node n); //convert a gate expression to a cnf expression

bool sat_solver(vector< vector<int> > cnf, int n);

bool beq(circuit a, circuit b);

/*
to call insert_gate function:
if it's an input or an output, then in1 and in2 are -1
for example, "input 2003" is "0 -1 -1 2003" ; "output 84" is "1 -1 -1 84"
*/

/*int main()
{
    int mode, out, in1, in2;
    vector<circuit> v;//all the circuit graph
    circuit ori_cir;
    circuit cur_cir;
    ori_cir.init();
    cur_cir.init();
    while(scanf("%d %d %d %d", &mode, &in1, &in2, &out)!=EOF){
        ori_cir.insert_gate(mode, in1, in2, out);
    }
    ori_cir.print_circuit();
    while(scanf("%d %d", &mode, &out)!=EOF){
        cur_cir=ori_cir;
        cur_cir.insert_fault(mode, out);
        cur_cir.print_circuit();
        v.push_back(cur_cir);
    }
    for(int i=0;i<v.size();i++){
        for(int j=0;j<v.size();j++){
            if(beq(v[i], v[j])) printf("%d %d\n", i, j);
        }
    }
    return 0;
}*/

#endif // EQUIVALENCE_H_INCLUDED
