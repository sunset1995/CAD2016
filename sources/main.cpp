#include <cstdio>
#include <vector>
#include <unordered_set>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include "sources/circuit.h"
#include "sources/equivalence.h"
#include "sources/fault.h"
#include "sources/parser.h"
#include "sources/SAT_solver.h"
#include "sources/simulator.h"

using namespace Minisat;


void compare_all(const Circuit &ori_cir, Fault &faults, const vector<int> &slot) {

    for(int i=0; i<slot.size(); ++i) {

        int mode = faults[slot[i]].mode;
        int net = faults[slot[i]].net;

        Circuit cir_1 = ori_cir;
        cir_1.insert_fault(mode, net);
        
        for(int j=i+1; j<slot.size(); ++j) {
            if( faults.same(slot[i], slot[j]) || faults.diff(slot[i], slot[j]) )
                continue;

            mode = faults[slot[j]].mode;
            net = faults[slot[j]].net;
            
            Circuit cir_2 = ori_cir;
            cir_2.insert_fault(mode, net);

            if( beq(cir_1, cir_2) )
                faults.setSame(slot[i], slot[j]);
            else
                faults.setDiff(slot[i], slot[j]);
        }

    }

}

int checkSum = 0;
void procTrashFault(const Circuit &ori_cir, Fault &faults, vector<bool> &trash) {
    // Find all trush fault(same as ori_cir)
    int trashLeader = -1;
    for(int i=0; i<faults.size(); ++i) {
        Circuit cir = ori_cir;
        cir.insert_fault(faults[i].mode, faults[i].net);

        if( beq(cir, ori_cir) ) {
            trash[i] = true;
            if( trashLeader==-1 )
                trashLeader = i;
        }
    }
    if( trashLeader!=-1 ) {
        for(int i=0; i<faults.size(); ++i) {
            if( trash[i] )
                faults.setSame(trashLeader, i);
            else
                faults.setDiff(trashLeader, i);
        }
    }
}


void procRandomSimulationTest(const Circuit &ori_cir, Fault &faults, const vector<bool> &trash) {
    
    struct Item{
        int fid, cid;
    };
    vector< vector<Item> > slot(1);
    
    // Init
    for(int i=0; i<faults.size(); ++i)
        if( !trash[i] )
            slot[0].push_back({i, -1});

    vector<Circuit> cir(slot[0].size(), ori_cir);
    for(int i=0; i<slot[0].size(); ++i) {
        cir[i].insert_fault(faults[slot[0][i].fid].mode, faults[slot[0][i].fid].net);
        slot[0][i].cid = i;
    }

    // Run simulation test
    vector<bool> input(ori_cir.input_cnt, 0);
    vector<bool> dff(ori_cir.dff.size(), 0);

    for(int __cnt=0; __cnt<20; ++__cnt) {
        
        for(int i=0; i<input.size(); ++i)
            input[i] = rand()&1;

        int to = slot.size();
        for(int i=0; i<to; ++i) {

            if( slot[i].size()<2 )
                continue;

            // pick slot[i][0] as leader
            vector<bool> leaderBits = simulate(cir[slot[i][0].cid], input, dff);
            
            // all fault different from leader must exit
            vector<Item> byebye;
            for(int j=1,top=1; j<slot[i].size(); ++j) {
                vector<bool> nowBits = simulate(cir[slot[i][j].cid], input, dff);
                if( nowBits != leaderBits )
                    byebye.emplace_back(slot[i][j]);
                else
                    slot[i][top++] = slot[i][j];
            }
            if( byebye.size() ) {
                slot[i].resize(slot[i].size() - byebye.size());
                slot.emplace_back(move(byebye));
            }
        }
    }

    printf("%d\n", (int)faults.size());
    for(int i=0; i<slot.size(); ++i) {
        checkSum += slot[i].size();
        printf("%d ", slot[i].size());
    }
    puts("");

    // Set different slot different
    for(int i=1; i<slot.size(); ++i)
        for(auto &id1 : slot[i])
            for(int j=0; j<i; ++j)
                for(auto &id2 : slot[j])
                    faults.setDiff(id1.fid, id2.fid);
}


int main(int argv, char **argc) {
    
    srand(time(NULL));

    if( argv<3 ) {
        puts("Not enough parameter");
        return 1;
    }

    freopen("identical_fault_pairs.txt", "w", stdout);

    ISC_parser testdata;

    Circuit ori_cir = testdata.parse_isc_file(argc[1]);
    Fault faults = testdata.parse_fault_file(argc[2]);

    ori_cir.dfs();


    
    vector<bool> trash(faults.size(), false);

    procTrashFault(ori_cir, faults, trash);

    procRandomSimulationTest(ori_cir, faults, trash);
    int trashcnt = 0;
    for(int i=0; i<trash.size(); ++i)
        trashcnt += trash[i],
        checkSum += trash[i];
    printf("%d\n", trashcnt);
    printf("%d vs. %d\n", faults.size(), checkSum);

/*
    // Greedy partition falut into multiple slot
    vector< vector<int> > slot;
    const vector<node> &cir = ori_cir.circuit;

    for(int i=0; i<faults.size(); ++i) {

        int id1 = ori_cir.gate_trans(faults[i].net);
        int sid = -1;

        for(int j=0; j<slot.size() && sid==-1; ++j) {
            
            int id2 = ori_cir.gate_trans(faults[slot[j][0]].net);
            sid = j;

            if( cir[id1].fanout.size() != cir[id2].fanout.size() ) {
                sid = -1;
                continue;
            }
            for(auto out : cir[id1].fanout)
                if( cir[id2].fanout.find(out) == cir[id2].fanout.end() ) {
                    sid = -1;
                    break;
                }
        }

        if( sid==-1 )
            slot.push_back({i});
        else
            slot[sid].push_back(i);

    }
    
    for(const auto &vec : slot)
        compare_all(ori_cir, faults, vec);


    // Print result
    vector< pair<int,int> > ans = faults.result();
    for(int i=0; i<ans.size(); ++i)
        printf("%d %d\n", ans[i].first, ans[i].second);
    fflush(stdout);
*/
    return 0;
}
