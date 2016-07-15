#include "sources/equivalence.h"

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
    else if(mode==2||mode==3){//and nand
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
    else if(mode==8||mode==9){// not buff
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
    return ret;
}

bool sat_solver(vector< vector<int> > cnf, int n)
{
    return 0;
}

bool beq(circuit a, circuit b)
{
    circuit miter=a;
    miter.output.clear();
	//create miter
	//deal with internal nodes
    for(int i=1;i<b.circuit.size();i++){
        node n=b.circuit[i];
        if(n.mode==0){
            miter.circuit[i].mode=8;
            n.mode=8;
            miter.circuit[i].in1=i+a.cnt+b.cnt+a.output.size();
            n.in1=i+a.cnt+b.cnt+a.output.size();
            miter.cnt++;
        }
        else{
            n.in1+=a.cnt;
            n.in2+=a.cnt;
        }
        n.out+=a.cnt;
        miter.circuit.push_back(n);
        miter.cnt++;
        miter.gate_cnt++;
    }
    //add buffers

	//add external xor gates
    for(int i=0;i<b.output.size();i++){
        node n;
        n.mode=6;
        n.in1=a.output[i];
        n.in2=b.output[i]+a.cnt;
        n.out=miter.cnt+1;
        miter.cnt++;
        miter.gate_cnt++;
        miter.circuit.push_back(n);
        miter.output.push_back(n.out);
    }
    miter.print_circuit();
	//convert the whole miter circuit to cnf expression
    vector< vector<int> > cnf;
    vector< vector<int> > tmp;
    vector<int> sum;
    for(int i=0;i<miter.circuit.size();i++){
        if(miter.circuit[i].mode!=0){// not primary input
            tmp=convert(miter.circuit[i]);
            for(int j=0;j<tmp.size();j++) cnf.push_back(tmp[j]);
        }
        else{// primary input
            if(miter.circuit[i].sa0){ //stuck at 0, so add one literal clause of it
                sum.clear();
                sum.push_back(-miter.circuit[i].out);
                cnf.push_back(sum);
            }
            else if(miter.circuit[i].sa1){ // stuck at 1, so add one liter clause of it
                sum.clear();
                sum.push_back(miter.circuit[i].out);
                cnf.push_back(sum);
            }
        }
    }
    // "OR" the xor gates
    for(int i=0;i<miter.output.size();i++){
        sum.push_back(miter.output[i]);
    }
    cnf.push_back(sum);
    // print the cnf expressions
    for(int i=0;i<cnf.size();i++){
        printf("(");
        for(int j=0;j<cnf[i].size();j++){
            printf("%dv", cnf[i][j]);
        }
        printf(")^");
    }
    if(sat_solver(cnf, miter.cnt)==0) return 1;// SAT SOLVER
    return 0;
}
