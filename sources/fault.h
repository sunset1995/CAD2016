#ifndef __FAULT_H
#define __FAULT_H

#include <cstring>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <unordered_set>
using namespace std;

class Fault {
public:
    Fault() {
        faults.reserve(4096);
    }

    struct fault {
        int fid, net, mode, group, minfid;
        unordered_set<int> diff;
    };
    vector<fault> faults;

    inline int size() {
        return faults.size();
    }

    fault& operator [] (const int id) {
        return faults[id];
    }

    // fault id, net id, fault name
    void addFault(int, int, const char*);

    // fault operation
    int group(int);
    bool same(int, int);
    bool diff(int, int);
    void setSame(int, int);
    void setDiff(int, int);
    
    // return final fault group result
    vector< pair<int,int> > result();

private:
    // conver fault name(str) to fault id(int)
    int faultName2faultMode(const char*);
};

#endif
