#ifndef __FAULT_H
#define __FAULT_H

#include <cstring>
#include <vector>
#include <algorithm>
#include <cstdlib>
using namespace std;

class Fault {
public:
    Fault() {
        faults.reserve(4096);
    }

    inline int size() {
        return faults.size();
    }

    inline int getNet(int id) const {
        return faults[id].net;
    }

    inline int getMode(int id) const {
        return faults[id].mode;
    }

    inline void heuristicSort() {
        for(int i=0; i<2000; ++i) {
            int a = rand() % size();
            int b = rand() % size();
            swap(faults[a], faults[b]);
        }
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
    struct fault {
        int id, net, mode, group;
    };
    vector<fault> faults;

    // for disjoint set
    vector<int> group;

    // conver fault name(str) to fault id(int)
    int faultName2faultMode(const char*);
};

#endif
