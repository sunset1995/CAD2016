#include "sources/parser.h"

circuit ISC_parser::parse_isc_file(const char *filename) {


	circuit ret;
	ret.init();

	// open file
	FILE *in = fopen(filename, "r");
	if( in==nullptr ) {
		puts("ISC file not found!");
		return ret;
	}
	char str[256];
	int  signal_id[3];
	
	// parse file
	while( fgets(str, 256, in)!=nullptr ) {
		trim_comment_newline(str);
		if( all_blank(str) )
			continue;
		get_nums(str, signal_id);
		if     ( strstr(str, "INPUT")!=nullptr )
			ret.insert_gate(0, -1, -1, signal_id[0]);
		else if( strstr(str, "OUTPUT")!=nullptr )
			ret.insert_gate(1, -1, -1, signal_id[0]);
		else if( strstr(str, "AND")!=nullptr )
			ret.insert_gate(2, signal_id[1], signal_id[2], signal_id[0]);
		else if( strstr(str, "NAND")!=nullptr )
			ret.insert_gate(3, signal_id[1], signal_id[2], signal_id[0]);
		else if( strstr(str, "OR")!=nullptr )
			ret.insert_gate(4, signal_id[1], signal_id[2], signal_id[0]);
		else if( strstr(str, "NOR")!=nullptr )
			ret.insert_gate(5, signal_id[1], signal_id[2], signal_id[0]);
		else if( strstr(str, "XOR")!=nullptr )
			ret.insert_gate(6, signal_id[1], signal_id[2], signal_id[0]);
		else if( strstr(str, "NXOR")!=nullptr )
			ret.insert_gate(7, signal_id[1], signal_id[2], signal_id[0]);
		else if( strstr(str, "BUFF")!=nullptr )
			ret.insert_gate(8, signal_id[1], -1, signal_id[0]);
		else if( strstr(str, "NOT")!=nullptr )
			ret.insert_gate(9, signal_id[1], -1, signal_id[0]);
	}
	fclose(in);

	return ret;
}

void ISC_parser::trim_comment_newline(char *str) {
	for(int i=0; str[i]!='\0'; ++i)
		if( str[i]=='#' || str[i]=='\n' ) {
			str[i] = '\0';
			return;
		}
}

bool ISC_parser::all_blank(const char *str) {
	for(int i=0; str[i]!='\0'; ++i)
		if( str[i]!=' ' )
			return false;
	return true;
}

void ISC_parser::get_nums(const char *str, int *num) {
	int top=-1, val=0;
	bool readingNum=false;
	for(int i=0; str[i]!='\0'; ++i) {
		if( str[i]>='0' && str[i]<='9' ) {
			val = val*10 + str[i] - '0';
			readingNum = true;
		}
		else if( readingNum ) {
			num[++top] = val;
			val = 0;
			readingNum = false;
		}
	}
}
