#include "sources/simulator.h"


vector<bool> simulate(const Circuit &cir, const vector<bool> &input) {
    
    int cnt = cir.circuit.size();
    vector<unsigned char> nds(cnt, 2);

    // Set input
    for(int i=0, j=0; i<cnt; ++i)
        if( cir.circuit[i].mode == 0 )
            nds[i] = input[j++];

    // Coculate output
    vector<bool> output(cir.output.size());
    for(int i=0; i<cir.output.size(); ++i)
        output[i] = getSignal(cir, nds, cir.output[i]);

    return output;
}


bool getSignal(const Circuit &cir, vector<unsigned char> &dp, int nowAt) {
    if( dp[nowAt]==0 ) return false;
    if( dp[nowAt]==1 ) return true;

    if( cir.circuit[nowAt].sa0 ) {
        dp[nowAt] = 0;
        return false;
    }

    if( cir.circuit[nowAt].sa1 ) {
        dp[nowAt] = 1;
        return true;
    }

    int mode = cir.circuit[nowAt].mode;
    int in1 = cir.circuit[nowAt].in1;
    int in2 = cir.circuit[nowAt].in2;
    bool neg = cir.circuit[nowAt].neg;

    // buff/not
    if( in2 == -1 ) {
        dp[nowAt] = getSignal(cir, dp, in1);
        dp[nowAt] ^= (mode==9);
        dp[nowAt] ^= neg;
        return dp[nowAt];
    }

    bool res_1 = getSignal(cir, dp, in1);
    bool res_2 = getSignal(cir, dp, in2);

    if( mode==2 )
        dp[nowAt] = res_1 & res_2;
    else if( mode==3 )
        dp[nowAt] = !(res_1 & res_2);
    else if( mode==4 )
        dp[nowAt] = res_1 | res_2;
    else if( mode==5 )
        dp[nowAt] = !(res_1 | res_2);
    else if( mode==6 )
        dp[nowAt] = res_1 ^ res_2;
    else if( mode==7 )
        dp[nowAt] = !(res_1 ^ res_2);

    dp[nowAt] ^= neg;

    return dp[nowAt];
}
