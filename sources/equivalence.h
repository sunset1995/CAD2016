#include<cstdio>
#include<cstdlib>
#include<map>
#include<vector>
#include"circuit.h"

using namespace std;

vector< vector<int> > convert(node n) //convert a gate expression to a cnf expression
{
    vector< vector<int> > ret;
    vector<int> tmp;
    int mode, in1, in2, out;
    bool neg, sa0, sa1;
    mode=n.mode;
    in1=n.in1;
    in2=n.in2;
    out=n.out;
    neg=n.neg;
    sa0=n.sa0;
    sa1=n.sa1;
    if(mode==2||mode==3){//and nand
        tmp.clear();// !a !b c
        tmp.push_back(-in1);
        tmp.push_back(-in2);
        if((mode==2&&neg==0)||(mode==3&&neg==1)) tmp.push_back(out);
        else tmp.push_back(-out);
        ret.push_back(tmp);
        tmp.clear();// a !c
        tmp.push_back(in1);
        if((mode==2&&neg==0)||(mode==3&&neg==1)) tmp.push_back(-out);
        else tmp.push_back(out);
        ret.push_back(tmp);
        tmp.clear();// b !c
        tmp.push_back(in2);
        if((mode==2&&neg==0)||(mode==3&&neg==1)) tmp.push_back(-out);
        else tmp.push_back(out);
        ret.push_back(tmp);
    }
    else if(mode==4 || mode==5){//or nor
        tmp.clear();// a b !c
        tmp.push_back(in1);
        tmp.push_back(in2);
        if((mode==4&&neg==0)||(mode==5&&neg==1)) tmp.push_back(-out);
        else tmp.push_back(out);
        ret.push_back(tmp);
        tmp.clear();// !a c
        tmp.push_back(-in1);
        if((mode==4&&neg==0)||(mode==5&&neg==1)) tmp.push_back(out);
        else tmp.push_back(-out);
        ret.push_back(tmp);
        tmp.clear();// !b c
        tmp.push_back(-in2);
        if((mode==4&&neg==0)||(mode==5&&neg==1)) tmp.push_back(out);
        else tmp.push_back(-out);
        ret.push_back(tmp);
    }
    else if(mode==6||mode==7){//xor or nxor
        tmp.clear();// !a !b !c
        tmp.push_back(-in1);
        tmp.push_back(-in2);
        if((mode==6&&neg==0)||(mode==7&&neg==1)) tmp.push_back(-out);
        else tmp.push_back(out);
        ret.push_back(tmp);
        tmp.clear();// a b !c
        tmp.push_back(in1);
        tmp.push_back(in2);
        if((mode==6&&neg==0)||(mode==7&&neg==1)) tmp.push_back(-out);
        else tmp.push_back(out);
        ret.push_back(tmp);
        tmp.clear();// a !b c
        tmp.push_back(in1);
        tmp.push_back(-in2);
        if((mode==6&&neg==0)||(mode==7&&neg==1)) tmp.push_back(out);
        else tmp.push_back(-out);
        ret.push_back(tmp);
        tmp.clear();// !a b c
        tmp.push_back(-in1);
        tmp.push_back(in2);
        if((mode==6&&neg==0)||(mode==7&&neg==1)) tmp.push_back(out);
        else tmp.push_back(-out);
        ret.push_back(tmp);
    }
    if(mode==8||mode==9){// not buff
        tmp.clear();// !a !c
        tmp.push_back(-in1);
        if((mode==9&&neg==0)||(mode==8&&neg==1)) tmp.push_back(-out);
        else tmp.push_back(out);
        ret.push_back(tmp);
        tmp.clear();// a c
        tmp.push_back(in1);
        if((mode==9&&neg==0)||(mode==8&&neg==1)) tmp.push_back(out);
        else tmp.push_back(-out);
        ret.push_back(tmp);
    }
    if(sa0){
        tmp.clear();
        tmp.push_back(-out);
        ret.push_back(tmp);
    }
    else if(sa1){
        tmp.clear();
        tmp.push_back(out);
        ret.push_back(tmp);
    }
    return ret;
}

bool beq(circuit a, circuit b)
{
    circuit miter=a;
    miter.output.clear();
	//create miter
	//deal with internal nodes
    for(int i=0;i<b.circuit.size();i++){
        node n=b.circuit[i];
        if(n.mode!=0){//not a primary input node
            if(n.in1>a.input_cnt) n.in1+=a.gate_cnt;//not primary input
            if(n.in2>a.input_cnt) n.in2+=a.gate_cnt;//not primary input
            n.out+=a.gate_cnt;
            miter.circuit.push_back(n);
            miter.cnt++;
            miter.gate_cnt++;
        }
    }
	//add external xor gates
    for(int i=0;i<b.output.size();i++){
        node n;
        n.mode=6;
        n.in1=a.output[i];
        n.in2=b.output[i]+a.gate_cnt;
        n.out=miter.cnt+1;
        miter.cnt++;
        miter.gate_cnt++;
        miter.circuit.push_back(n);
        miter.output.push_back(n.out);
    }
    //miter.print_circuit();
	//convert the whole miter circuit to cnf expression
    vector< vector<int> > cnf;
    vector< vector<int> > tmp;
    for(int i=0;i<miter.circuit.size();i++){
        if(miter.circuit[i].mode!=0){
            tmp=convert(miter.circuit[i]);
            for(int j=0;j<tmp.size();j++) cnf.push_back(tmp[j]);
        }
    }
    // "OR" the xor gates
    vector<int> sum;
    cnt++;
    for(int i=0;i<miter.output.size();i++){
        sum.push_back(miter.output[i]);
    }
    sum.push_back(-cnt);
    cnf.push_back(sum);
    for(int i=0;i<miter.output.size();i++){
        sum.clear();
        sum.push_back(-miter.output[i]);
        sum.push_back(cnt);
        cnf.push_back(sum);
    }
    sum.clear();
    sum.push_back(cnt);
    cnf.push_back(sum);
    if(sat_solver(cnf, cnt)==0) return 1;// SAT SOLVER
    return 0;
}

void solve()
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
        //cur_cir.print_circuit();
        v.push_back(cur_cir);
    }
    for(int i=0;i<v.size();i++){
        for(int j=0;j<v.size();j++){
            if(beq(v[i], v[j])) printf("%d %d\n", i, j);
        }
    }
    return 0;
}

