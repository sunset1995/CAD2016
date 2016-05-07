#ifndef __PARSER_H
#define __PARSER_H

#include <cstdio>
#include <cstring>
#include "AIG.h"
using namespace std;

class ISC_parser {
public:
	// input filename; output AIG
	AIG parse_isc_file(const char*);
private:
	// self used function
	void trim_comment_newline(char*);
	bool all_blank(const char*);
	void get_nums(const char*, int*);
};

#endif
