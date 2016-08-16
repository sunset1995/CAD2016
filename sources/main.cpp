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

vector<Circuit> cir;
vector< vector<int> > slots;

void compare_all(const Circuit &ori_cir, Fault &faults, const vector<int> &slot) {

    for(int i=0; i<slot.size(); ++i) {
        
        for(int j=i+1; j<slot.size(); ++j) {
            if( faults.same(slot[i], slot[j]) || faults.diff(slot[i], slot[j]) )
                continue;

            if( beq(cir[slot[i]], cir[slot[j]]) )
                faults.setSame(slot[i], slot[j]);
            else
                faults.setDiff(slot[i], slot[j]);
        }

    }

}


void procTrashFault(const Circuit &ori_cir, Fault &faults) {
    // Find all trush fault(same as ori_cir)
    int trashLeader = -1;
    for(int i=0; i<faults.size(); ++i) {
        if( trashLeader!=-1 && faults.diff(i, trashLeader) )
            continue;

        if( beq(cir[i], ori_cir) ) {
            faults[i].trash = true;
            if( trashLeader==-1 )
                trashLeader = i;
        }
    }

    if( trashLeader!=-1 ) {
        for(int i=0; i<faults.size(); ++i) {
            if( faults[i].trash )
                faults.setSame(trashLeader, i);
            else
                faults.setDiff(trashLeader, i);
        }
    }
}


void procRandomSimulationTest(const Circuit &ori_cir, Fault &faults) {
    
    slots.clear();
    slots.resize(1);
    
    // Init
    for(int i=0; i<faults.size(); ++i)
        slots[0].push_back(i);

    // Run simulation test
    vector<bool> input(ori_cir.input_cnt, 0);
    vector<bool> dff(ori_cir.dff.size(), 0);

    for(int __cnt=0; __cnt<20; ++__cnt) {
        
        for(int i=0; i<input.size(); ++i)
            input[i] = rand()&1;

        bool good = false;

        int to = slots.size();
        for(int i=0; i<to; ++i) {

            if( slots[i].size()<2 )
                continue;

            // pick slot[i][0] as leader
            vector<bool> leaderBits = simulate(cir[slots[i][0]], input, dff);
            
            // all fault different from leader must exit
            vector<int> byebye;
            for(int j=1,top=1; j<slots[i].size(); ++j) {
                vector<bool> nowBits = simulate(cir[slots[i][j]], input, dff);
                if( nowBits != leaderBits )
                    byebye.emplace_back(slots[i][j]);
                else
                    slots[i][top++] = slots[i][j];
            }
            if( byebye.size() ) {
                slots[i].resize(slots[i].size() - byebye.size());
                slots.emplace_back(move(byebye));
                good = true;
            }
        }
        if( !good )
            break;
    }

    // Set different slot different
    for(int i=1; i<slots.size(); ++i)
        for(auto &id1 : slots[i])
            for(int j=0; j<i; ++j)
                for(auto &id2 : slots[j])
                    faults.setDiff(id1, id2);
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


    cir.resize(faults.size(), ori_cir);
    for(int i=0; i<faults.size(); ++i)
        cir[i].insert_fault(faults[i].mode, faults[i].net);


    procRandomSimulationTest(ori_cir, faults);
    procTrashFault(ori_cir, faults);


    // Greedy partition falut into multiple slot
    const vector<node> &cir = ori_cir.circuit;

    for(int i=0; i<slots.size(); ++i) {
        if( slots[i].size()<2 )
            continue;

        int id1 = ori_cir.gate_trans(faults[slots[i][0]].net);

        // all fault different from leader must exit
        vector<int> byebye;
        int top=1;
        for(int j=1; j<slots[i].size(); ++j) {
            if( faults[slots[i][j]].trash )
                continue;
            int id2 = ori_cir.gate_trans(faults[slots[i][j]].net);
            if( cir[id1].fanout != cir[id2].fanout )
                byebye.emplace_back(slots[i][j]);
            else
                slots[i][top++] = slots[i][j];
        }

        if( byebye.size() ) {
            slots[i].resize(top);
            slots.emplace_back(move(byebye));
        }
    }

    
    for(const auto &vec : slots)
        compare_all(ori_cir, faults, vec);


    // Print result
    vector< pair<int,int> > ans = faults.result();
    for(int i=0; i<ans.size(); ++i)
        printf("%d %d\n", ans[i].first, ans[i].second);
    fflush(stdout);

    return 0;
}
