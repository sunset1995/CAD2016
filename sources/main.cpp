#include <cstdio>
#include <vector>
#include "sources/circuit.h"
#include "sources/equivalence.h"
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
	circuit ori_cir = testdata.parse_isc_file(argc[1]);
	ori_cir.print_circuit();
	circuit cir_1;
	circuit cir_2;
	int mode, id;
	scanf("%d %d", &mode, &id);
	cir_1=ori_cir.insert_fault(mode, id);
	scanf("%d %d", &mode, &id);
	cir_2=ori_cir.insert_fault(mode, id);
	beq(cir_1, cir_2);
	return 0;
}
