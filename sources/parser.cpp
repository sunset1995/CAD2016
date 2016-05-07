#include "parser.h"

AIG ISC_parser::parse_isc_file(const char *filename) {
	AIG ret;
	FILE *in = fopen(filename, "r");
	if( in==nullptr ) {
		puts("ISC file not found!");
		return ret;
	}
	char str[256];
	int  signal_id[3];
	while( fgets(str, 256, in)!=nullptr ) {
		trim_comment_newline(str);
		if( all_blank(str) )
			continue;
		get_nums(str, signal_id);
		if( strstr(str, "OUTPUT")!=nullptr || strstr(str, "INPUT")!=nullptr )
			printf("%d\n", signal_id[0]);
		else if( strstr(str, "BUFF")!=nullptr || strstr(str, "NOT")!=nullptr )
			printf("%d %d\n", signal_id[0], signal_id[1]);
		else
			printf("%d %d %d\n", signal_id[0], signal_id[1], signal_id[2]);
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
