
// Get nowAt nds signal
// (an assignment of input, current nodes state, circuit)
bool getSignal(vector<char> &nds, const Circuit &cir, int nowAt);

bool randomInputTest(const Circuit &a, const Circuit &b);

bool getSignal(vector<char> &nds, const Circuit &cir, int nowAt) {
    if( nds[nowAt]==0 ) return false;
    if( nds[nowAt]==1 ) return true;

    if( cir.circuit[nowAt].sa0 ) {
        nds[nowAt] = 0;
        return false;
    }

    if( cir.circuit[nowAt].sa1 ) {
        nds[nowAt] = 1;
        return true;
    }

    int mode = cir.circuit[nowAt].mode;
    int in1 = cir.circuit[nowAt].in1;
    int in2 = cir.circuit[nowAt].in2;
    bool neg = cir.circuit[nowAt].neg;

    // buff/not
    if( in2 == -1 ) {
        nds[nowAt] = getSignal(nds, cir, in1);
        nds[nowAt] ^= (mode==9);
        nds[nowAt] ^= neg;
        return nds[nowAt];
    }

    bool res_1 = getSignal(nds, cir, in1);
    bool res_2 = getSignal(nds, cir, in2);

    if( mode==2 )
        nds[nowAt] = res_1 & res_2;
    else if( mode==3 )
        nds[nowAt] = !(res_1 & res_2);
    else if( mode==4 )
        nds[nowAt] = res_1 | res_2;
    else if( mode==5 )
        nds[nowAt] = !(res_1 | res_2);
    else if( mode==6 )
        nds[nowAt] = res_1 ^ res_2;
    else if( mode==7 )
        nds[nowAt] = !(res_1 ^ res_2);

    nds[nowAt] ^= neg;

    return nds[nowAt];
}

bool randomInputTest(const Circuit &a, const Circuit &b) {
    /*
    *   return true if pass test
    */

    // 0 low; 1 hight; 2 undetermined
    int cnt = a.circuit.size();
    vector<char> nds_a(cnt, 2);
    vector<char> nds_b(cnt, 2);

    for(int i=0; i<cnt; ++i)
        if( a.circuit[i].mode == 0 )
            nds_a[i] = nds_b[i] = (rand()&1);

    for(int i=0; i<a.output.size(); ++i)
        if( getSignal(nds_a, a, a.output[i]) != getSignal(nds_b, b, b.output[i]) )
            return false;

    return true;
}

    for(int i=0; i<1000; ++i)
        if( !randomInputTest(a, b) )
            return false;
    return true;
