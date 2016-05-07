#include <cstdio>
#include <vector>
#include "AIG.h"
#include "parser.h"

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
	testdata.parse_isc_file(argc[1]);

	/*
	AIG test;
	test.add_input (100);
	test.add_input (101);
	test.add_input (228);
	test.add_input (229);
	test.add_input (105);
	test.add_output(301);
	test.add_xor_gate (100,101,502);
	test.add_xor_gate (228,229,503);
	test.add_nxor_gate(502,503,504);
	test.add_and_gate (504,105,301);


	for(int i=0; i<(1<<test.input_length()); ++i) {
		for(int j=0; j<test.input_length(); ++j) {
			if( i&(1<<j) )
				test.set_input(j, true);
			else
				test.set_input(j, false);
		}
		show_binary(i, test.input_length());
		putchar(' ');
		test.coculate();
		for(int j=0; j<test.output_length(); ++j)
			printf("%d", test.get_output(j));
		puts("");
	}
	*/
	return 0;
}
