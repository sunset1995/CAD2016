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

bool getSignal(vector<char> &nds, const Circuit &cir, int nowAt) {
    if( nds[nowAt]==0 ) return false;
    if( nds[nowAt]==1 ) return true;

    if( cir.circuit[nowAt].sa0 ) {
        nds[nowAt] = 0;
        return false;
    }

    if( cir.circuit[nowAt].sa1 ) {
        nds[nowAt] = 1;
        return true;
    }

    int mode = cir.circuit[nowAt].mode;
    int in1 = cir.circuit[nowAt].in1;
    int in2 = cir.circuit[nowAt].in2;
    bool neg = cir.circuit[nowAt].neg;

    // buff/not
    if( in2 == -1 ) {
        nds[nowAt] = getSignal(nds, cir, in1);
        nds[nowAt] ^= neg;
        return nds[nowAt];
    }

    bool res_1 = getSignal(nds, cir, in1);
    bool res_2 = getSignal(nds, cir, in2);

    if( mode==2 )
        nds[nowAt] = res_1 & res_2;
    else if( mode==3 )
        nds[nowAt] = !(res_1 & res_2);
    else if( mode==4 )
        nds[nowAt] = res_1 | res_2;
    else if( mode==5 )
        nds[nowAt] = !(res_1 | res_2);
    else if( mode==6 )
        nds[nowAt] = res_1 ^ res_2;
    else if( mode==7 )
        nds[nowAt] = !(res_1 ^ res_2);

    nds[nowAt] ^= neg;

    return nds[nowAt];
}

bool randomInputTest(const Circuit &a, const Circuit &b) {
    /*
    *   return true if pass test
    */

    // 0 low; 1 hight; 2 undetermined
    vector<char> nds_1(a.cnt, 2);
    vector<char> nds_2(b.cnt, 2);

    for(int i=0; i<a.cnt; ++i)
        if( a.circuit[i].mode == 0 )
            nds_1[i] = nds_2[i] = (rand()&1);

    for(int i=0; i<a.output.size(); ++i)
        if( getSignal(nds_1, a, a.output[i]) != getSignal(nds_2, b, b.output[i]) )
            return false;

    return true;
}

bool beq(const Circuit &a, const Circuit &b)
{
    for(int i=0; i<5; ++i)
        if( !randomInputTest(a, b) )
            return false;

    Circuit miter=a;
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
            miter.gate_cnt++;
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
        n.out=a.cnt+b.cnt+1+i;
        miter.cnt++;
        miter.gate_cnt++;
        miter.circuit.push_back(n);
        miter.output.push_back(n.out);
    }
	//convert the whole miter circuit to cnf expression
    vector< vector<int> > cnf;
    vector< vector<int> > tmp;
    vector<int> sum;
    for(int i=1;i<miter.circuit.size();i++){
        tmp=convert(miter.circuit[i]);
        for(int j=0;j<tmp.size();j++) cnf.push_back(tmp[j]);
    }
    // "OR" the xor gates
    for(int i=0;i<miter.output.size();i++){
        sum.push_back(miter.output[i]);
    }
    cnf.push_back(sum);

    return !SAT_solver(cnf, miter.cnt);
}
