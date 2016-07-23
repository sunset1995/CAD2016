#include "sources/circuit.h"

Circuit::Circuit()
{
    init();
}

void Circuit::init()
{
    circuit.clear();
    node n;
    circuit.push_back(n);
    mp.clear();
    input_cnt=0;
    gate_cnt=0;
    cnt=0;
}

int Circuit::gate_trans(int gate)// to map the node id to an interger
{
    if(gate==-1) return -1;
    if(mp.find(gate)!=mp.end()) return mp[gate];
    node n;
    circuit.push_back(n);
    cnt++;
    mp[gate]=cnt;
    return cnt;
}

void Circuit::insert_output(int out)// to record which gates are primary outputs
{
    out=gate_trans(out);
    output.push_back(out);
    circuit[out].PO=output.size()-1;
    //circuit[out]
}

void Circuit::insert_gate(int mode, int in1, int in2, int out)
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
    n.PO=circuit[out].PO;
    //circuit.push_back(n);
    circuit[out]=n;
}

void Circuit::insert_fault(int mode, int id)
{
    id=gate_trans(id);
    fault_id=id;
    if(mode>=2 && mode<=9) circuit[id].mode=mode;
    else{
        if(mode==0){//stuck at 0
            circuit[id].sa0=1;
        }
        else if(mode==1){//stuck at 1
            circuit[id].sa1=1;
        }
        else if(mode==10){// negation
            circuit[id].neg=1;
        }
        if(mode==0 || mode==1 || mode==10){//stuck at 0 or 1 or neg
            if(circuit[id].mode==0){//primary input
                //add a new buff gate
                node n;
                n.in1=id;
                n.in2=-1;
                n.out=cnt+1;
                fault_id=cnt+1;
                n.mode=8;
                if(mode==0) n.sa0=1;
                else if(mode==1) n.sa1=1;
                else if(mode==10) n.neg=1;
                circuit.push_back(n);
                cnt++;
                gate_cnt++;
                circuit[id].sa0=circuit[id].sa1=circuit[id].neg=0;
                //to broadcast all the fanout gates to change its input id to buff's
                for(int i=1;i<cnt;i++){
                    if(circuit[i].in1==id) circuit[i].in1=n.out;
                    if(circuit[i].in2==id) circuit[i].in2=n.out;
                }
                //to broadcast all the fanout POs to insert this buff gate as its upstream
                //to tell this buff gate which POs it will influence
                unordered_set<int>::iterator it;
                for(it=circuit[id].fanout.begin();it!=circuit[id].fanout.end();it++){
                    int fout=*it;
                    circuit[cnt].fanout.insert(fout);
                    circuit[fout].fanin.insert(cnt);
                }
                //if it's also a primary output
                //the new PO will be this buff gate
                if(circuit[id].PO!=-1){
                    int index=circuit[id].PO;
                    output[index]=cnt;//replace
                    for(it=circuit[id].fanout.begin();it!=circuit[id].fanout.end();it++){
                        if(*it==id){
                            circuit[id].fanout.erase(*it);
                            circuit[id].fanout.insert(cnt);
                            break;
                        }
                    }
                    circuit[cnt].fanin=circuit[id].fanin;
                    circuit[id].fanin.clear();
                    circuit[cnt].fanout=circuit[id].fanout;
                    circuit[cnt].PO=circuit[id].PO;
                    circuit[id].PO=-1;
                }
            }
        }
    }
}

void Circuit::dfs()
{
    for(int i=0;i<output.size();i++){
        stack<int> s;
        int index=output[i];
        int cur=output[i];
        node n=circuit[cur];
        s.push(cur);
        while(!s.empty()){
            cur=s.top();
            s.pop();
            if(circuit[index].fanin.find(cur)!=circuit[index].fanin.end()) continue;
            circuit[index].fanin.insert(cur);
            circuit[cur].fanout.insert(index);
            int n1=circuit[cur].in1;
            int n2=circuit[cur].in2;
            if(n1>0) s.push(n1);
            if(n2>0) s.push(n2);
        }
    }
}

void Circuit::add_xor_gates(vector<int> out)
{
    output.clear();
    for(int i=0;i<out.size()/2;i++){
        node n;
        n.in1=out[i];
        n.in2=out[i+out.size()/2];
        n.out=cnt+1;
        cnt++;
        gate_cnt++;
        n.mode=6;
        circuit.push_back(n);
        output.push_back(cnt);
    }
}

void Circuit::print_circuit()
{
    unordered_set<int>::iterator it;
    printf("cnt=%d input_cnt=%d gate_cnt=%d\n", cnt, input_cnt, gate_cnt);
    for(int i=1;i<circuit.size();i++){
        printf("mode=%d in1=%d in2=%d out=%d neg=%d sa0=%d sa1=%d PO=%d\n", circuit[i].mode, circuit[i].in1, circuit[i].in2, circuit[i].out, circuit[i].neg, circuit[i].sa0, circuit[i].sa1, circuit[i].PO);
        /*printf("fanin: ");
        for(it=circuit[i].fanin.begin();it!=circuit[i].fanin.end();it++) printf("%d ", *it);
        printf("\nfanout: ");
        for(it=circuit[i].fanout.begin();it!=circuit[i].fanout.end();it++) printf("%d ", *it);
        puts("");*/
    }
    for(int i=0;i<output.size();i++){
        printf("output %d\n", output[i]);
    }
}
