#include "sources/SAT_solver.h"

vector<int> SAT_solver(vector< vector<int> > clauses, int n) {

    solver S;
    S.init(clauses, n);
    S.solve(solver::HEURISTIC_VSIDS | solver::HEURISTIC_MOM_INIT);
    vector<int> result = S.result();

    if( result[0] ) {
        for (int i=1; i<=n; i++)
            result[i] = result[i]>0;
    }
    else result.clear();
    
    return result;

}
