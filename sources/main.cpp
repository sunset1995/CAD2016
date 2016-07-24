#include <cstdio>
#include <vector>
#include <ctime>
#include <cstdlib>
#include "sources/circuit.h"
#include "sources/equivalence.h"
#include "sources/fault.h"
#include "sources/parser.h"
#include "sources/SAT_solver.h"

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

int main(int argv, char **argc) {
    
    srand(time(NULL));

    if( argv<3 ) {
        puts("Not enough parameter");
        return 1;
    }

    ISC_parser testdata;

    Circuit ori_cir = testdata.parse_isc_file(argc[1]);
    Fault faults = testdata.parse_fault_file(argc[2]);

    ori_cir.dfs();

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


    // Compare faluts inside one slot
    for(const auto &vec : slot)
        compare_all(ori_cir, faults, vec);
    slot.clear();


    // Compare all group
    vector<int> universe_slot(faults.size());
    for(int i=0; i<faults.size(); ++i)
        universe_slot[i] = i;
    compare_all(ori_cir, faults, universe_slot);


    // Print result
    vector< pair<int,int> > ans = faults.result();
    for(int i=0; i<ans.size(); ++i)
        printf("%d %d\n", ans[i].first, ans[i].second);

    return 0;
}
