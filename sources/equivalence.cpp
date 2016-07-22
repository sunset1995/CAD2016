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

Circuit join(const Circuit &a, const Circuit &b)
{
    set<int> PO;
    set<int> upstream;
    set<int>::iterator it;
    set<int>::iterator PO_it;
    set<int>::iterator upstream_it;
    node fault_node;
    node PO_node;
    node cur_node;
    int in1, in2, out, mode;
    int id;
    fault_node=a.circuit[a.fault_id];

    //to make a POs set
    for(it=fault_node.fanout.begin();it!=fault_node.fanout.end();it++){
        PO.insert(*it);
    }
    if(b.fault_id!=a.fault_id){
        fault_node=b.circuit[b.fault_id];
        for(it=fault_node.fanout.begin();it!=fault_node.fanout.end();it++){
            PO.insert(*it);
        }
    }

    Circuit miter;
    for(int i=0;i<2;i++){
        int cnt;
        if(i==0) cnt=a.cnt;
        else cnt=b.cnt;
        // to make a upstream set
        for(PO_it=PO.begin();PO_it!=PO.end();PO_it++){
            id=*PO_it;
            if(id<=cnt){//this id maybe circuit_b's PI stuck buff gate, we don't insert it
                if(i==0){
                    PO_node=a.circuit[id];
                    miter.insert_output(id);
                }
                else{
                    PO_node=b.circuit[id];
                    miter.insert_output(-(id+1));
                }
                for(upstream_it=PO_node.fanin.begin();upstream_it!=PO_node.fanin.end();upstream_it++){
                    upstream.insert(*upstream_it);
                }
            }
        }
        //insert all the upstream nodes
        for(upstream_it=upstream.begin();upstream_it!=upstream.end();upstream_it++){
            out=*upstream_it;
            if(out>cnt) continue; //this id maybe circuit_b's PI stuck buff gate, we don't insert it
            if(i==0) cur_node=a.circuit[out];
            else cur_node=b.circuit[out];
            in1=cur_node.in1;
            in2=cur_node.in2;
            mode=cur_node.mode;
            if(i==1){
                if(mode==0) continue;
                out=-(out+1);
                if(b.circuit[in1].mode!=0) in1=-(in1+1);
                if(in2>0&&b.circuit[in2].mode!=0) in2=-(in2+1);
            }
            printf("insert %d %d %d %d\n", mode, in1, in2, out);
            miter.insert_gate(mode, in1, in2, out);
            if(cur_node.sa0) miter.insert_fault(0, out);
            if(cur_node.sa1) miter.insert_fault(1, out);
            if(cur_node.neg) miter.insert_fault(10, out);
        }
    }

    return miter;
}

bool beq(const Circuit &a, const Circuit &b)
{
    for(int i=0; i<5; ++i)
        if( !randomInputTest(a, b) )
            return false;

    Circuit miter=join(a, b);

    //add external xor gates
    miter.add_xor_gates(miter.output);
    //miter.print_circuit();

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
