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
	return 0;
}
