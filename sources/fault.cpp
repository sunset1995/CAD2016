#include "sources/fault.h"

void Fault::addFault(int _id, int _net, const char *_mode) {
    faults.push_back({
        _id,
        _net,
        faultName2faultMode(_mode),
        0
    });
    faults.back().group = faults.size() - 1;
}

int Fault::find(int id) {
    return (faults[id].group==id)? id : (faults[id].group = find(faults[id].group));
}

void Fault::join(int a, int b) {
    a = find(a);
    b = find(b);
    if( faults[a].id < faults[b].id )
        faults[b].group = a;
    else
        faults[a].group = b;
}

vector< pair<int,int> > Fault::result() {
    vector< pair<int,int> > ret;
    ret.reserve(size());
    for(int i=0,root; i<size(); ++i) {
        root = find(i);
        if( i!=root )
            ret.push_back({faults[root].id, faults[i].id});
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
