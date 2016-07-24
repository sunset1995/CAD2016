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

    for(int i=faults.size()-1; i>=0; --i) {

        int mode = faults[i].mode;
        int net = faults[i].net;

        Circuit cir_1 = ori_cir;
        cir_1.insert_fault(mode, net);
        
        for(int j=i-1; j>=0; --j) {
            if( faults.same(i, j) || faults.diff(i, j) )
                continue;

            mode = faults[j].mode;
            net = faults[j].net;
            
            Circuit cir_2 = ori_cir;
            cir_2.insert_fault(mode, net);

            if( beq(cir_1, cir_2) )
                faults.setSame(i, j);
            else
                faults.setDiff(i, j);
        }
    }

    vector< pair<int,int> > ans = faults.result();
    for(int i=0; i<ans.size(); ++i)
        printf("%d %d\n", ans[i].first, ans[i].second);

    return 0;
}
