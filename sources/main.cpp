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

void printVector(vector<bool> &vec)
{
    printf("VEC:: ");
    for(int i=0;i<vec.size();i++){
        printf("%d ", vec[i]?1:0);
    }
    puts("");
}


void procRandomSimulationTest(const Circuit &ori_cir, Fault &faults, int sim_space_span = 20, int sim_time_span = 20) {
    
    slots.clear();
    slots.resize(1);
    
    // Init
    for(int i=0; i<faults.size(); ++i)
        slots[0].push_back(i);

    // Run at most 20 rounds
    for(int __round=0; __round<sim_space_span; ++__round) {
        // Run simulation testdata
        int __time_span_max = (ori_cir.seq ? sim_time_span : 1);
        // If split new slot or not
        bool good = false;

        vector<bool> input(ori_cir.input_cnt, 0);
        vector< vector<bool> > dff(cir.size(), vector<bool>(ori_cir.dff.size(), 0));

        for(int __time_span=0; __time_span < __time_span_max; ++__time_span){
            for(int i=0; i<input.size(); ++i)
                input[i] = rand()&1;
            
            // Store slot leader output
            pair< vector<bool>, vector<bool> > tmp;
            vector< vector<bool> > output(slots.size());
            vector< vector<bool> > dff_next(cir.size(), vector<bool>(ori_cir.dff.size(), 0));
            
                    
            // No need to check new generated slot in same round
            int to = slots.size();
            for(int i=0; i<to; ++i) {

                if( slots[i].size()<2 )
                    continue;

                // pick slot[i][0] as leader
                tmp = simulate(cir[slots[i][0]], input, dff[slots[i][0]]);
                output[i] = tmp.first;
                dff_next[slots[i][0]] = tmp.second;
                
                // all fault different from leader must exit
                int start = slots.size();
                int sz = 1;
                for(int j=1; j<slots[i].size(); ++j) {
                    tmp  = simulate(cir[slots[i][j]], input, dff[slots[i][j]]);

                    vector<bool> nowBits = tmp.first;
                    dff_next[slots[i][j]] = tmp.second;

                    if( nowBits == output[i] ) {
                        slots[i][sz++] = slots[i][j];
                        continue;
                    }
                    good = true;

                    bool findNewMaster = false;
                    for(int k=start; k<slots.size(); ++k)
                        if( nowBits == output[k] ) {
                            slots[k].emplace_back(slots[i][j]);
                            findNewMaster = true;
                            break;
                        }
                    if( !findNewMaster ) {
                        slots.push_back({slots[i][j]});
                        output.emplace_back(move(nowBits));
                    }
                }
                slots[i].resize(sz);
            }            
            dff = dff_next;
        }
        // if( !good )
        //     break;
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
    int sim_space_span = 20, sim_time_span = 20;

    if( argv<3 ) {
        puts("Not enough parameter");
        return 1;
    }
    if(argv == 5){
        sim_space_span = atoi(argc[3]);
        sim_time_span = atoi(argc[4]);
        // printf("Read sim argument: SpaceSpan: %d,  TimeSpan: %d\n", sim_space_span, sim_time_span);
    }

    freopen("identical_fault_pairs.txt", "w", stdout);

    ISC_parser testdata;

    Circuit ori_cir = testdata.parse_isc_file(argc[1]);
    Fault faults = testdata.parse_fault_file(argc[2]);


    ori_cir.dfs();
    // ori_cir.print_circuit();

    cir.resize(faults.size(), ori_cir);
    for(int i=0; i<faults.size(); ++i)
        cir[i].insert_fault(faults[i].mode, faults[i].net);


    procRandomSimulationTest(ori_cir, faults, sim_space_span, sim_time_span);
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
