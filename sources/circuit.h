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
};

class circuit
{
public:
    vector<node> circuit; // store all nodes
    vector<int> output; // which nodes are output?
    map<int, int> mp; // gate name map to integer
    int cnt;
    void init();
    void insert_gate(int mode, int in1, int in2, int out);
    void insert_output(int out);
    int gate_trans(int gate);
    void insert_fault(int mode, int id);
    void print_circuit();
    void print_cnf();
    vector< vector<int> > circuit_to_cnf(int offset);
};

void circuit::init()
{
    circuit.clear();
    mp.clear();
    cnt=0;
}

int circuit::gate_trans(int gate)
{
    if(gate==-1) return -1;
    if(mp.find(gate)!=mp.end()) return mp[gate];
    mp[gate]=cnt;
    cnt++;
    return cnt-1;
}

void circuit::insert_output(int out)
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
    in1=gate_trans(in1);
    in2=gate_trans(in2);
    out=gate_trans(out);
    node n;
    n.mode=mode;
    n.in1=in1;
    n.in2=in2;
    n.out=out;
    n.neg=n.sa0=n.sa1=0;
    circuit.push_back(n);
}

void circuit::insert_fault(int mode, int id)
{
    id=gate_trans(id);
    if(mode>=2 && mode<=9) circuit[id].mode=mode;
    else{
        if(mode==0){
            circuit[id].sa0=1;
            circuit[id].sa1=0;
        }
        else if(mode==1){
            circuit[id].sa1=1;
            circuit[id].sa0=0;
        }
        else if(mode==10){
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
    for(int i=0;i<circuit.size();i++){
        printf("mode=%d in=%d in=%d out=%d i=%d\n", circuit[i].mode, circuit[i].in1, circuit[i].in2, circuit[i].fanout, i);
    }
    for(int i=0;i<output.size();i++){
        printf("output %d\n", output[i]);
    }
}

vector< vector<int> > circuit::circuit_to_cnf(int offset)
{
    vector< vector<int> > ret;
    vector<int> tmp;
    int mode, in1, in2, out;
    bool neg, sa0, sa1;
    for(int i=0;i<circuit.size();i++){
        mode=circuit[i].mode;
        in1=circuit[i].in1+offset;
        in2=circuit[i].in2+offset;
        out=circuit[i].out+offset;
        neg=circuit[i].neg;
        sa0=circuit[i].sa0;
        sa1=circuit[i].sa1;
        if(mode==2||mode==3){//and nand
            tmp.clear();
            if(!(circuit[in1].sa0||circuit[in2].sa0)){// !in1 or !in2 or out
                if(!circuit[in1].sa1) tmp.push_back(-in1);
                if(!circuit[in2].sa1) tmp.push_back(-in2);
                if((mode==2&&neg==0)||(mode==3&&neg==1)) tmp.push_back(out);
                else tmp.push_back(-out);
            }
            ret.push_back(tmp);
            tmp.clear();
            if(!circuit[in1].sa1){// in1 or !out
                if(!circuit[in1].sa0) tmp.push_back(in1);
                if((mode==2&&neg==0)||(mode==3&&neg==1)) tmp.push_back(-out);
                else tmp.push_back(out);
            }
            ret.push_back(tmp);
            tmp.clear();
            if(!circuit[in2].sa1){// in2 or !out
                if(!circuit[in2].sa0) tmp.push_back(in2);
                if((mode==2&&neg==0)||(mode==3&&neg==1)) tmp.push_back(-out);
                else tmp.push_back(out);
            }
            ret.push_back(tmp);
            return ret;
        }
        if(mode==4 || mode==5){//or nor
            tmp.clear();
            if(!(circuit[in1].sa1||circuit[in2].sa1)){// in1 or in2 or !out
                if(!circuit[in1].sa0) tmp.push_back(in1);
                if(!circuit[in2].sa0) tmp.push_back(in2);
                if((mode==4&&neg==0)||(mode==5&&neg==1)) tmp.push_back(-out);
                else tmp.push_back(out);
            }
            ret.push_back(tmp);
            tmp.clear();
            if(!circuit[in1].sa0){// !in1 or out
                if(!circuit[in1].sa1) tmp.push_back(-in1);
                if((mode==4&&neg==0)||(mode==5&&neg==1)) tmp.push_back(out);
                else tmp.push_back(-out);
            }
            ret.push_back(tmp);
            tmp.clear();
            if(!circuit[in2].sa0){// !in2 or out
                if(!circuit[in2].sa1) tmp.push_back(-in2);
                if((mode==4&&neg==0)||(mode==5&&neg==1)) tmp.push_back(out);
                else tmp.push_back(-out);
            }
            ret.push_back(tmp);
            return ret;
        }
        if(mode==6||mode==7){//xor or nxor
            tmp.clear();
            if(!(circuit[in1].sa0||circuit[in2].sa0)){// !in1 or !in2 or !out
                if(!circuit[in1].sa1) tmp.push_back(-in1);
                if(!circuit[in2].sa1) tmp.push_back(-in2);
                if((mode==6&&neg==0)||(mode==7&&neg==1)) tmp.push_back(-out);
                else tmp.push_back(out);
            }
            ret.push_back(tmp);
            tmp.clear();
            if(!(circuit[in1].sa1||circuit[in2].sa1)){// in1 or in2 or !out
                if(!circuit[in1].sa0) tmp.push_back(in1);
                if(!circuit[in2].sa0) tmp.push_back(in2);
                if((mode==6&&neg==0)||(mode==7&&neg==1)) tmp.push_back(-out);
                else tmp.push_back(out);
            }
            ret.push_back(tmp);
            tmp.clear();
            if(!(circuit[in1].sa1||circuit[in2].sa0)){// in1 or !in2 or out
                if(!circuit[in1].sa0) tmp.push_back(in1);
                if(!circuit[in2].sa1) tmp.push_back(-in2);
                if((mode==6&&neg==0)||(mode==7&&neg==1)) tmp.push_back(out);
                else tmp.push_back(-out);
            }
            ret.push_back(tmp);
            tmp.clear();
            if(!(circuit[in1].sa0||circuit[in2].sa1)){// !in1 or in2 or out
                if(!circuit[in1].sa1) tmp.push_back(-in1);
                if(!circuit[in2].sa0) tmp.push_back(in2);
                if((mode==6&&neg==0)||(mode==7&&neg==1)) tmp.push_back(out);
                else tmp.push_back(-out);
            }
            ret.push_back(tmp);
            return ret;
        }
        if(mode==8||mode==9){// not buff
            tmp.clear();
            if(!circuit[in1].sa0){// !in1 or !out
                if(!circuit[in1].sa1) tmp.push_back(-in1);
                if((mode==9&&neg==0)||(mode==8&&neg==1)) tmp.push_back(-out);
                else tmp.push_back(out);
            }
            ret.push_back(ret);
            if(!circuit[in1].sa1){// in1 or out
                if(!circuit[in1].sa0) tmp.push_back(in1);
                if((mode==9&&neg==0)||(mode==8&&neg==1)) tmp.push_back(out);
                else tmp.push_back(-out);
            }
            ret.push_back(ret);
            return ret;
        }
        if(mode==0){//literal
            tmp.clear();
            if(neg==0) tmp.push_back(out);
            else tmp.push_back(-out);
            ret.push_back(tmp);
            return ret;
        }
    }
}


#endif // CIRCUIT_H_INCLUDED
