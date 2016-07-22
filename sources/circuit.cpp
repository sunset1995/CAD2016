#include "sources/circuit.h"

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

    //circuit.push_back(n);
    circuit[out]=n;
}

void Circuit::insert_fault(int mode, int id)
{
    id=gate_trans(id);
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
        if(mode==0 || mode==1){//stuck at 0 or 1
            if(circuit[id].mode==0){//primary input
                node n;
                n.in1=id;
                n.in2=-1;
                n.out=cnt+1;
                n.mode=8;
                if(mode==0) n.sa0=1;
                else n.sa1=1;
                circuit.push_back(n);
                cnt++;
                circuit[id].sa0=circuit[id].sa1=0;
                set<int>::iterator it;
                for(it=circuit[id].fanout.begin();it!=circuit[id].fanout.end();it++){
                    int fout=*it;
                    circuit[cnt].fanout.insert(fout);
                    circuit[fout].fanin.insert(cnt);
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
            printf("cur=%d\n", cur);
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

void Circuit::print_circuit()
{
    set<int>::iterator it;
    printf("cnt=%d input_cnt=%d gate_cnt=%d\n", cnt, input_cnt, gate_cnt);
    printf("circuit.size=%d\n", circuit.size());
    for(int i=1;i<circuit.size();i++){
        printf("mode=%d in1=%d in2=%d out=%d neg=%d sa0=%d sa1=%d i=%d\n", circuit[i].mode, circuit[i].in1, circuit[i].in2, circuit[i].out, circuit[i].neg, circuit[i].sa0, circuit[i].sa1, i);
        printf("fanin: ");
        for(it=circuit[i].fanin.begin();it!=circuit[i].fanin.end();it++) printf("%d ", *it);
        printf("\nfanout: ");
        for(it=circuit[i].fanout.begin();it!=circuit[i].fanout.end();it++) printf("%d ", *it);
        puts("");
    }
    for(int i=0;i<output.size();i++){
        printf("output %d\n", output[i]);
    }
}
