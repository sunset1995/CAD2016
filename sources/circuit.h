/*
2016 CAD
Circuit Equivalence Checking
Circuit graph transform to CNF expression

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
*/

#ifndef CIRCUIT_H_INCLUDED
#define CIRCUIT_H_INCLUDED

#include<cstdio>
#include<map>
#include<vector>

using namespace std;

struct node
{
    int mode;
    int in1;
    int in2;
    int out;
    bool neg;
    bool sa0;
    bool sa1;
    node(){neg=sa0=sa1=0;}
};

class circuit
{
public:
    vector<node> circuit; // store all nodes
    vector<int> output; // which nodes are output?
    map<int, int> mp;
    int cnt;
    int input_cnt;
    int gate_cnt;
    void init();
    void insert_gate(int mode, int in1, int in2, int out);
    void insert_output(int out);
    int gate_trans(int gate);
    void insert_fault(int mode, int id);
    void print_circuit();
};

void circuit::init()
{
    circuit.clear();
    node n;
    circuit.push_back(n);
    mp.clear();
    input_cnt=0;
    gate_cnt=0;
    cnt=0;
}

int circuit::gate_trans(int gate)// to map the node id to an interger
{
    if(gate==-1) return -1;
    if(mp.find(gate)!=mp.end()) return mp[gate];
    node n;
    circuit.push_back(n);
    cnt++;
    mp[gate]=cnt;
    return cnt;
}

void circuit::insert_output(int out)// to record which gates are primary outputs
{
    out=gate_trans(out);
    output.push_back(out);
}

void circuit::insert_gate(int mode, int in1, int in2, int out)
{
    if(mode==1){
        insert_output(out);
        return;
    }
    if(mode==0) input_cnt++;
    else gate_cnt++;
    in1=gate_trans(in1);
    in2=gate_trans(in2);
    out=gate_trans(out);
    node n;
    n.mode=mode;
    n.in1=in1;
    n.in2=in2;
    n.out=out;
    n.neg=n.sa0=n.sa1=0;
    //circuit.push_back(n);
    circuit[out]=n;
}

void circuit::insert_fault(int mode, int id)
{
    id=gate_trans(id);
    if(mode>=2 && mode<=9) circuit[id].mode=mode;
    else{
        if(mode==0){//stuck at 0
            circuit[id].sa0=1;
            circuit[id].sa1=0;
        }
        else if(mode==1){//stuck at 1
            circuit[id].sa1=1;
            circuit[id].sa0=0;
        }
        else if(mode==10){// negation
            circuit[id].neg^=1;
            if(circuit[id].sa0){
                circuit[id].sa0=0;
                circuit[id].sa1=1;
            }
            else if(circuit[id].sa1){
                circuit[id].sa1=0;
                circuit[id].sa0=1;
            }
        }
    }
}

void circuit::print_circuit()
{
    printf("cnt=%d input_cnt=%d gate_cnt=%d\n", cnt, input_cnt, gate_cnt);
    for(int i=1;i<circuit.size();i++){
        printf("mode=%d in=%d in=%d out=%d i=%d\n", circuit[i].mode, circuit[i].in1, circuit[i].in2, circuit[i].out, i);
    }
    for(int i=0;i<output.size();i++){
        printf("output %d\n", output[i]);
    }
}

#endif // CIRCUIT_H_INCLUDED
