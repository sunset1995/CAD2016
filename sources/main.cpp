#include <cstdio>
#include <vector>
#include "sources/parser.h"
#include "sources/SAT_solver.h"

using namespace Minisat;

void show_binary(int val, int nbits) {
	for(int i=nbits-1; i>=0; --i)
		putchar(val&(1<<i)? '1' : '0');
}

int main(int argv, char **argc) {
	if( argv<2 ) {
		puts("Not enough parameter");
		return 1;
	}
	ISC_parser testdata;
	testdata.parse_isc_file(argc[1]);

	return 0;
}
