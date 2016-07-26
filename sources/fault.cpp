#include "sources/fault.h"

void Fault::addFault(int _id, int _net, const char *_mode) {
    fault now;
    now.fid = _id;
    now.net = _net;
    now.mode = faultName2faultMode(_mode);
    now.group = faults.size();
    now.minfid = _id;
    faults.emplace_back(now);
}

int Fault::group(int i) {
    if( faults[i].group==i )
        return i;
    return faults[i].group = group(faults[i].group);
}

bool Fault::same(int a, int b) {
    return group(a)==group(b);
}

bool Fault::diff(int a, int b) {
    a = group(a);
    b = group(b);
    return faults[a].diff.find(b)!=faults[a].diff.end();
}

void Fault::setSame(int a, int b) {
    a = group(a);
    b = group(b);
    if( a==b )
        return;
    faults[b].group = a;
    faults[a].minfid = min(faults[a].minfid, faults[b].minfid);
    for(auto id : faults[b].diff)
        faults[a].diff.insert(id);
    faults[b].diff.clear();
}

void Fault::setDiff(int a, int b) {
    a = group(a);
    b = group(b);
    faults[a].diff.insert(b);
    faults[b].diff.insert(a);
}

vector< pair<int,int> > Fault::result() {
    vector< pair<int,int> > ret;
    ret.reserve(size());
    for(int i=0; i<size(); ++i) {
        int minfid = faults[group(i)].minfid;
        if( faults[i].fid != minfid )
            ret.push_back({minfid, faults[i].fid});
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
