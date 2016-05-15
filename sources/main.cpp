#include <cstdio>
#include <vector>
#include "sources/AIG.h"
#include "sources/parser.h"
#include "sources/SAT_solver.h"

using namespace Minisat;

void show_binary(int val, int nbits) {
	for(int i=nbits-1; i>=0; --i)
		putchar(val&(1<<i)? '1' : '0');
}

int main(int argv, char **argc) {
	try {
// Extra options:
		IntOption    verb   ("MAIN", "verb",   "Verbosity level (0=silent, 1=some, 2=more).", 0, IntRange(0, 2));
		IntOption    cpu_lim("MAIN", "cpu-lim","Limit on CPU time allowed in seconds.\n", 0, IntRange(0, INT32_MAX));
		IntOption    mem_lim("MAIN", "mem-lim","Limit on memory usage in megabytes.\n", 0, IntRange(0, INT32_MAX));

		Solver S;

		S.verbosity = verb;

// Try to set resource limits:
		if (cpu_lim != 0) limitTime(cpu_lim);
		if (mem_lim != 0) limitMemory(mem_lim);

// Input start
		int vars, clauses;
		printf("Number of variables: ");
		scanf("%d", &vars);
		printf("Number of clauses: ");
		scanf("%d", &clauses);

		puts("\n   Each clauses per line end with 0.");
		printf("!! Available variable are +/-1~%d\n", vars);

		while (vars >= S.nVars()) S.newVar();
		for(int i=0,v; i<clauses; ++i) {
			vec<Lit> lits;
			while( scanf("%d", &v)!=EOF && v )
				if( v>0 )
					lits.push( mkLit(abs(v)-1) );
				else
					lits.push( ~mkLit(abs(v)-1) );
			S.addClause_(lits);
		}
// Input done

		if (!S.simplify()) {
			puts("Solved by unit propagation");
			S.printStats();
			puts("");
			puts("UNSATISFIABLE");
			exit(20);
		}

		vec<Lit> dummy;
		lbool ret = S.solveLimited(dummy);

		S.printStats();
		puts("");

		if( ret == l_True ) puts("SATISFIABLE");
		else if( ret == l_False ) puts("UNSATISFIABLE");
		else puts("INDETERMINATE");

		exit(ret == l_True ? 10 : ret == l_False ? 20 : 0);

	} catch (OutOfMemoryException&){
		puts("Out of memory");
		puts("INDETERMINATE");
		exit(0);
	}

	/*
	if( argv<3 ) {
		puts("Not enough parameter");
		return 1;
	}
	ISC_parser testdata;
	AIG ori_AIG(testdata.parse_isc_file(argc[1]));

	for(int i=0; i<(1<<ori_AIG.input_length()); ++i) {
		for(int j=0; j<ori_AIG.input_length(); ++j) {
			if( i&(1<<j) )
				ori_AIG.set_input(j, true);
			else
				ori_AIG.set_input(j, false);
		}
		show_binary(i, ori_AIG.input_length());
		putchar(' ');
		ori_AIG.coculate();
		for(int j=0; j<ori_AIG.output_length(); ++j)
			printf("%d", ori_AIG.get_output(j));
		puts("");
	}
	*/
	return 0;
}
