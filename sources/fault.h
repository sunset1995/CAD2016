#ifndef __FAULT_H
#define __FAULT_H

#include <cstring>
#include <vector>
#include <algorithm>
using namespace std;

class Fault {
public:
    Fault() {
        id.reserve(4096);
        net.reserve(4096);
        mode.reserve(4096);
        group.reserve(4096);
    }

    inline int size() {
        return id.size();
    }

    inline int getNet(int id) const {
        return net[id];
    }

    inline int getMode(int id) const {
        return mode[id];
    }

    // fault id, net id, fault name
    void addFault(int, int, const char*);

    // implemented disjoint set
    int find(int);
    void join(int, int);
    
    // return final fault group result
    vector< pair<int,int> > result();
private:
    // fault id ; net id ; fault mode
    vector<int> id;
    vector<int> net;
    vector<int> mode;

    // for disjoint set
    vector<int> group;

    // conver fault name(str) to fault id(int)
    int faultName2faultMode(const char*);
};

#endif
