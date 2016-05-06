#include "AIG.h"

int AIG::input_length() {
	return I_tachi.size();
}

int AIG::output_length() {
	return O_tachi.size();
}

void AIG::set_input(int nth, bool val) {
	nds[I_tachi[nth]].val = val;
}

int AIG::get_output(int nth) {
	return nds[O_tachi[nth]].val;
}

void AIG::add_input(int I) {
	if( idx.find(I)!=idx.end() )
		return;
	nds.push_back(Node());
	nds.back().I_port = true;
	idx[I] = (nds.size()-1);
	I_tachi.emplace_back(nds.size()-1);
}

void AIG::add_output(int O) {
	if( idx.find(O)!=idx.end() )
		return;
	nds.push_back(Node());
	nds.back().O_port = true;
	idx[O] = (nds.size()-1);
	O_tachi.emplace_back(nds.size()-1);
}

void AIG::add_buff_gate(int I, int O) {
	add_gate_proc(I,false,I,false,O,false);
}

void AIG::add_not_gate(int I, int O) {
	add_gate_proc(I,true,I,true,O,false);
}

void AIG::add_and_gate(int I1, int I2, int O) {
	add_gate_proc(I1,false,I2,false,O,false);
}

void AIG::add_nand_gate(int I1, int I2, int O) {
	add_gate_proc(I1,false,I2,false,O,true);
}

void AIG::add_or_gate(int I1, int I2, int O) {
	// a+b = !(!a * !b)
	add_gate_proc(I1,true,I2,true,O,true);
}

void AIG::add_nor_gate(int I1, int I2, int O) {
	add_gate_proc(I1,true,I2,true,O,false);
}

void AIG::add_xor_gate(int I1, int I2, int O) {
	I1 = netid_to_nodeid(I1);
	I2 = netid_to_nodeid(I2);
	O  = netid_to_nodeid(O);

	nds.push_back(Node());
	int t1 = nds.size() - 1;
	nds.push_back(Node());
	int t2 = nds.size() - 1;

	add_gate_proc(I1,true ,I2,false,t1,false, IS_NDS_ID);
	add_gate_proc(I1,false,I2,true ,t2,false, IS_NDS_ID);
	add_gate_proc(t1,true ,t2,true ,O ,true , IS_NDS_ID);
}

void AIG::add_nxor_gate(int I1, int I2, int O) {
	I1 = netid_to_nodeid(I1);
	I2 = netid_to_nodeid(I2);
	O  = netid_to_nodeid(O);

	nds.push_back(Node());
	int t1 = nds.size() - 1;
	nds.push_back(Node());
	int t2 = nds.size() - 1;

	add_gate_proc(I1,true ,I2,true ,t1,false, IS_NDS_ID);
	add_gate_proc(I1,false,I2,false,t2,false, IS_NDS_ID);
	add_gate_proc(t1,true ,t2,true ,O ,true , IS_NDS_ID);
}

int AIG::netid_to_nodeid(int netid) {
	auto it = idx.find(netid);
	if( it!=idx.end() )
		return it->second;
	nds.push_back(Node());
	return idx[netid] = (nds.size()-1);
}

void AIG::add_gate_proc(
		int I1, bool I1_inv,
		int I2, bool I2_inv,
		int O , bool O_inv,
		bool id_is_net
		) {

	if( id_is_net ) {
		I1 = netid_to_nodeid(I1);
		I2 = netid_to_nodeid(I2);
		O  = netid_to_nodeid(O);
	}
	nds[I1].out.emplace_back(O);
	if( I2!=I1 )
		nds[I2].out.emplace_back(O);
	nds[O].in1 = {I1, I1_inv};
	nds[O].in2 = {I2, I2_inv};
	nds[O].invert_all_out = O_inv;
}

void AIG::coculate() {
	vector<bool> visited(nds.size()+4, false);
	for(int i=0; i<O_tachi.size(); ++i)
		coculate(O_tachi[i], visited);
}

void AIG::coculate(int nowAt, vector<bool> &visited) {
	visited[nowAt] = true;
	AIG::Node &now = nds[nowAt];
	int s1 = now.in1.source;
	int s2 = now.in2.source;
	if( !visited[s1] && !nds[s1].I_port )
		coculate(s1, visited);
	if( !visited[s2] && !nds[s2].I_port )
		coculate(s2, visited);
	now.val = (nds[s1].val ^ now.in1.invert)
	        & (nds[s2].val ^ now.in2.invert);
	now.val ^= now.invert_all_out;
}
