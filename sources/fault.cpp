#include "sources/fault.h"

void Fault::addFault(int _id, int _net, const char *_mode) {
    id.emplace_back(_id);
    net.emplace_back(_net);
    mode.emplace_back(faultName2faultMode(_mode));
    group.emplace_back(group.size());
}

int Fault::find(int id) {
    return (group[id]==id)? id : (group[id] = find(group[id]));
}

void Fault::join(int a, int b) {
    a = find(a);
    b = find(b);
    if( id[a]<id[b] )
        group[b] = a;
    else
        group[a] = b;
}

vector< pair<int,int> > Fault::result() {
    vector< pair<int,int> > ret;
    ret.reserve(id.size());
    for(int i=0,root; i<id.size(); ++i) {
        root = find(i);
        if( i!=root )
            ret.push_back({id[root], id[i]});
    }
    sort(ret.begin(), ret.end());
    return ret;
}


// Private self used function
int Fault::faultName2faultMode(const char *_mode) {
    if( strcmp(_mode, "SA0")==0 )
        return 0;
    else if( strcmp(_mode, "SA1")==0 )
        return 1;
    else if( strcmp(_mode, "NEG")==0 )
        return 10;
    else if( strcmp(_mode, "RDOB_AND")==0 )
        return 2;
    else if( strcmp(_mode, "RDOB_NAND")==0 )
        return 3;
    else if( strcmp(_mode, "RDOB_OR")==0 )
        return 4;
    else if( strcmp(_mode, "RDOB_NOR")==0 )
        return 5;
    else if( strcmp(_mode, "RDOB_XOR")==0 )
        return 6;
    else if( strcmp(_mode, "RDOB_NXOR")==0 )
        return 7;
    else if( strcmp(_mode, "RDOB_BUFF")==0 )
        return 8;
    else if( strcmp(_mode, "RDOB_NOT")==0 )
        return 9;
    return 0;
}
