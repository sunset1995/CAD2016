/*
2016 CAD
Circuit graphs

Gate mode definitions:
0 input
1 output
2 and gate
3 nand gate
4 or gate
5 nor gate
6 xor gate
7 nxor gate
8 buff gate
9 not gate

Fault mode definitons:
2~9 are the same as above
0 sa0
1 sa1
10 neg
*/

#ifndef CIRCUIT_H_INCLUDED
#define CIRCUIT_H_INCLUDED

#include <cstdio>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <vector>

using namespace std;

struct node
{
    int mode;
    int in1;//if it's dff, in1 is rst
    int in2;//if it's dff, in2 is enable
    int din;//if it's dff, din is D
    int out;
    bool neg;
    bool sa0;
    bool sa1;
    int PO;//the index in output vector. if it isn't PO, than -1
    unordered_set<int> fanin;//if it is a PO node, which node will affect it?
    unordered_set<int> fanout;//this node will affect which PO node?
    node(){neg=sa0=sa1=0;PO=-1;
    fanin.clear();
    fanout.clear();}
};

class Circuit
{
public:
    vector<node> circuit; // store all nodes
    vector<int> output; // which nodes are output?
    vector<int> dff; //which nodes are dffs?
    vector<int> dff_in;
    unordered_map<int, int> mp;
    int cnt;
    int input_cnt;
    int gate_cnt;
    int fault_id;
    bool seq;//if it's a sequential circuit
    Circuit();
    void init();
    void insert_gate(int mode, int in1, int in2, int din, int out);
    void insert_output(int out);
    void dfs();
    int gate_trans(int gate);
    void insert_fault(int mode, int id);
    void add_xor_gates(vector<int> out);
    void print_circuit();
};

#endif // CIRCUIT_H_INCLUDED
