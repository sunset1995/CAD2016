#include "sources/SAT_solver.h"

bool SAT_solver(vector< vector<int> > clauses, int n) {
    

    clock_t t = clock();


    Solver S;

    // Setting CNF formular
    while(n > S.nVars())
        S.newVar();
    vector< vec<Lit> > lits(clauses.size());
    puts("Clauses:");
    for(int i=0,v; i<clauses.size(); ++i) {
        vector<int> &nowClause = clauses[i];
        for(int j=0; j<nowClause.size(); ++j) {
            printf("%4d ", nowClause[j]);
            lits[i].push( mkLit(abs(nowClause[j])-1, nowClause[j]<0) );
        }
        puts("");

        if( lits[i].size()==1 )
            S.addClause(lits[i][0]);
        else if( lits[i].size()==2 )
            S.addClause(lits[i][0], lits[i][1]);
        else if( lits[i].size()==3 )
            S.addClause(lits[i][0], lits[i][1], lits[i][2]);
        else
            S.addClause(lits[i]);
    }
    puts("----");

    // Try to solve via simplify
    if (!S.simplify())
        return false;

    // Solve
    vec<Lit> dummy;
    lbool ret = S.solveLimited(dummy);

    if( ret==l_True ) {
        for (int i = 0; i<S.nVars(); i++)
            if (S.model[i] != l_Undef)
                printf("%s%s%d", (i==0)?"":" ", (S.model[i]==l_True)?"":"-", i+1);
        puts("");
        puts("------");
    }

    printf("SAT_solver: %d clauses, %d vars in %f ms\n", clauses.size(), n, 1000.0*t/CLOCKS_PER_SEC);

    // Return result
    if( ret==l_True )
        return true;
    else if( ret==l_False )
        return false;
    else
        return false; // Indeterminate
}