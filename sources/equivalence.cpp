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

bool SEQ_SAT(Circuit CCT, vector< vector<int> > CNF, int N)
{
    vector<bool> state;
    unordered_set< vector<bool> > st;
    vector< vector<int> > cnf=CNF;
    vector<int> qout;//all ppi index
    vector<int> din;//all ppo index
    vector<int> tmp;
    vector<bool> sat;
    qout=CCT.dff;
    for(int i=0;i<qout.size();i++){
        int index=qout[i];
        din.push_back(CCT.circuit[index].din);
    }
    //test if the reset state will sat
    for(int i=0;i<qout.size();i++){
        tmp.clear();
        tmp.push_back(-qout[i]);
        cnf.push_back(tmp);
    }
    if(SAT_solver(cnf, N).size()>0) return 1;
    //tuei r chiu chi tz
    for(int i=0;i<qout.size();i++) cnf.pop_back();
    sat=SAT_solver(cnf, N);
    if(sat.size()==0) return 0;
    state.clear();
    for(int i=0;i<qout.size();i++){
        int id=qout[i];
        state.push_back(sat[id]);
    }
    //po don't necessary 1
    cnf.pop_back();
    //trace back
    while(st.find(state)==st.end()){
        st.insert(state);
        for(int i=0;i<din.size();i++){
            tmp.clear();
            if(state[i]) tmp.push_back(din[i]);
            else tmp.push_back(-din[i]);
            cnf.push_back(tmp);
        }
        //test if the reset state will sat
        for(int i=0;i<qout.size();i++){
            tmp.clear();
            tmp.push_back(-qout[i]);
            cnf.push_back(tmp);
        }
        if(SAT_solver(cnf, N).size()>0) return 1;
        //tuei r chiu chi tz
        for(int i=0;i<qout.size();i++) cnf.pop_back();
        sat=SAT_solver(cnf, N);
        if(sat.size()==0) return 0;
        state.clear();
        for(int i=0;i<qout.size();i++){
            int id=qout[i];
            state.push_back(sat[id]);
        }
    }
    return 0;
}

Circuit join(const Circuit &a, const Circuit &b)
{
    unordered_set<int> PO;
    unordered_set<int> upstream;
    unordered_set<int>::iterator it;
    unordered_set<int>::iterator PO_it;
    unordered_set<int>::iterator upstream_it;
    node fault_node;
    node PO_node;
    node cur_node;
    int in1, in2, din, out, mode;
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
            if(id>cnt){ //this id maybe another circuit's PI stuck buff gate, we take its origin signal
                if(i==0) id=b.circuit[id].in1;
                else id=a.circuit[id].in1;
            }
            if(i==0){
                PO_node=a.circuit[id];
                miter.insert_output(id);
            }
            else{
                PO_node=b.circuit[id];
                if(PO_node.mode==0) miter.insert_output(id);
                else miter.insert_output(-(id+1));
            }
            for(upstream_it=PO_node.fanin.begin();upstream_it!=PO_node.fanin.end();upstream_it++){
                upstream.insert(*upstream_it);
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
            din=cur_node.din;
            mode=cur_node.mode;
            if(i==1){
                if(mode==0) continue;
                out=-(out+1);
                if(b.circuit[in1].mode!=0) in1=-(in1+1);
                if(in2>0&&b.circuit[in2].mode!=0) in2=-(in2+1);
                if(din>0&&b.circuit[din].mode!=0) din=-(din+1);
            }
            miter.insert_gate(mode, in1, in2, din, out);
            if(cur_node.sa0) miter.insert_fault(0, out);
            if(cur_node.sa1) miter.insert_fault(1, out);
            if(cur_node.neg) miter.insert_fault(10, out);
        }
    }
    if(a.seq) miter.seq=1;
    return miter;
}

bool beq(const Circuit &a, const Circuit &b)
{
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


   if(miter.seq==0) return SAT_solver(cnf, miter.cnt).empty();
   else return !SEQ_SAT(miter, cnf, miter.cnt);
}
