#include <cstdio>
#include <vector>
#include "sources/circuit.h"
#include "sources/equivalence.h"
#include "sources/fault.h"
#include "sources/parser.h"
#include "sources/SAT_solver.h"

using namespace Minisat;

void show_binary(int val, int nbits) {
    for(int i=nbits-1; i>=0; --i)
        putchar(val&(1<<i)? '1' : '0');
}

int main(int argv, char **argc) {
    
    if( argv<3 ) {
        puts("Not enough parameter");
        return 1;
    }
    ISC_parser testdata;

    Circuit ori_cir = testdata.parse_isc_file(argc[1]);
    Fault faults = testdata.parse_fault_file(argc[2]);
    ori_cir.print_circuit();

    for(int i=0; i<faults.size(); ++i) {
        if( faults.find(i)!=i )
            continue;
        
        int mode = faults.getMode(i);
        int id = faults.getNet(i);

        Circuit cir_1 = ori_cir;
        cir_1.insert_fault(mode, id);
        
        for(int j=1; j<faults.size(); ++j) {
            if( faults.find(j)!=j )
                continue;

            mode = faults.getMode(j);
            id = faults.getNet(j);
            
            Circuit cir_2 = ori_cir;
            cir_2.insert_fault(mode, id);

            if( beq(cir_1, cir_2) )
                faults.join(i, j);
        }
    }

    vector< pair<int,int> > ans = faults.result();
    for(int i=0; i<ans.size(); ++i)
        printf("%d %d\n", ans[i].first, ans[i].second);

    return 0;
}
