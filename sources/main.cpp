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

    freopen("identical_fault_pairs.txt", "w", stdout);

    ISC_parser testdata;

    Circuit ori_cir = testdata.parse_isc_file(argc[1]);
    Fault faults = testdata.parse_fault_file(argc[2]);

    //faults.heuristicSort();

    clock_t sat_time = 0;
    int cnt = 0, grounp_cnt = 0;

    for(int i=faults.size()-1; i>=0; --i) {
        if( faults.find(i)!=i )
            continue;
        
        ++grounp_cnt;

        int mode = faults.getMode(i);
        int id = faults.getNet(i);

        Circuit cir_1 = ori_cir;
        cir_1.insert_fault(mode, id);
        
        for(int j=i-1; j>=0; --j) {
            if( faults.find(j)!=j )
                continue;

            ++cnt;
            mode = faults.getMode(j);
            id = faults.getNet(j);
            
            Circuit cir_2 = ori_cir;
            cir_2.insert_fault(mode, id);

            clock_t t = clock();
            if( beq(cir_1, cir_2) )
                faults.join(i, j);
            sat_time += clock() - t;
        }
    }

    vector< pair<int,int> > ans = faults.result();
    for(int i=0; i<ans.size(); ++i)
        printf("%d %d\n", ans[i].first, ans[i].second);

    printf("group num %d\n", grounp_cnt);
    //printf("call SAT_solver %d times\n", cnt);
    //printf("Time for SAT_solver %.2fs\n", (double)sat_time / CLOCKS_PER_SEC);
    return 0;
}
