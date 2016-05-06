#ifndef __AIG_h_
#define __AIG_h_

#include <vector>
#include <unordered_map>
using namespace std;

class AIG {
public:
	enum {
		IS_NDS_ID = false,
		IS_NET_ID = true
	};
	struct Edge {
		int  source;
		bool invert;
	};
	struct Node {
		bool val;

		// <source, invert>
		Edge in1;
		Edge in2;

		// successor node id
		vector<int> out;
		bool invert_all_out = false;

		// bool
		bool I_port = false;
		bool O_port = false;
	};

	int  input_length ();
	int  output_length();
	void set_input    (int,bool);
	int  get_output   (int);
	void add_input    (int);
	void add_output   (int);
	void add_buff_gate(int,int);
	void add_not_gate (int,int);
	void add_and_gate (int,int,int);
	void add_nand_gate(int,int,int);
	void add_or_gate  (int,int,int);
	void add_nor_gate (int,int,int);
	void add_xor_gate (int,int,int);
	void add_nxor_gate(int,int,int);

	void coculate();
private:
	vector<Node> nds;
	vector<int>  I_tachi;
	vector<int>  O_tachi;

	// map outter net id to AIG node id
	unordered_map<int,int> idx;
	int netid_to_nodeid(int);

	// self used function
	void add_gate_proc(int,bool,int,bool,int,bool,bool=IS_NET_ID);
	void coculate(int,vector<bool>&);
};

#endif
