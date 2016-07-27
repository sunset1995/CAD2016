#include "sources/SAT_solver.h"

vector<int> SAT_solver(vector< vector<int> > clauses, int n) {


    Solver S;

    // Setting CNF formular
    while(n+1 > S.nVars())
        S.newVar();
    vector< vec<Lit> > lits(clauses.size());
    for(int i=0,v; i<clauses.size(); ++i) {
        vector<int> &nowClause = clauses[i];
        for(int j=0; j<nowClause.size(); ++j) {
            lits[i].push( mkLit(abs(nowClause[j]), nowClause[j]<0) );
        }

        if( lits[i].size()==1 )
            S.addClause(lits[i][0]);
        else if( lits[i].size()==2 )
            S.addClause(lits[i][0], lits[i][1]);
        else if( lits[i].size()==3 )
            S.addClause(lits[i][0], lits[i][1], lits[i][2]);
        else
            S.addClause(lits[i]);
    }


    vector<int> result;

    // Try to solve via simplify
    if (!S.simplify())
        return result;

    // Result
    if( S.solve() ) {
        result.resize(n+1);
        for (int i=0; i<=n; i++)
            result[i] = (S.model[i] == l_True);
    }
    
    return result;

}
