#include "sources/simulator.h"


vector<bool> simulate(const Circuit &cir, const vector<bool> &input, const vector<bool> &dff) {
    
    int cnt = cir.circuit.size();

    // 0 for low
    // 1 for high
    // 2 for undeterminate
    vector<unsigned char> nds(cnt, 3);

    // Preset
    for(int i=0,j=-1; i<cir.circuit.size(); ++i)
        if( cir.circuit[i].mode == 0 )
            nds[i] = input[++j];
    for(int i=0,j=-1; i<cir.circuit.size(); ++i)
        if( cir.circuit[i].mode == 10 )
            nds[i] = input[++j];

    // Coculate output
    vector<bool> output(cir.output.size());
    for(int i=0; i<cir.output.size(); ++i)
        output[i] = getSignal(cir, nds, cir.output[i]);

    return output;
}


struct SimRecurStack {
    const int nowAt;
    int state;
    SimRecurStack(int nowAt, int state)
    :nowAt(nowAt), state(state) {}
};
bool getSignal(const Circuit &cir, vector<unsigned char> &dp, const int initAt) {
    vector<SimRecurStack> stk;
    stk.reserve(32);
    stk.emplace_back(SimRecurStack(initAt, -1));

    while( stk.size() ) {
        const int nowAt = stk.back().nowAt;

        if( stk.back().state==-1 ) {
            if( dp[nowAt]==0 || dp[nowAt]==1 ) {
                stk.pop_back();
                continue;
            }

            // Detect cycle
            if( dp[nowAt]==2 ) {
                dp[nowAt] = rand()&1;
                stk.pop_back();
                continue;
            }
            dp[nowAt] = 2;


            if( cir.circuit[nowAt].sa0 ) {
                dp[nowAt] = 0;
                stk.pop_back();
                continue;
            }

            if( cir.circuit[nowAt].sa1 ) {
                dp[nowAt] = 1;
                stk.pop_back();
                continue;
            }

            stk.back().state = 0;
        }

        int mode = cir.circuit[nowAt].mode;
        int in1 = cir.circuit[nowAt].in1;
        int in2 = cir.circuit[nowAt].in2;
        bool neg = cir.circuit[nowAt].neg;

        if( stk.back().state == 0 ) {

            // buff/not
            if( in2 == -1 )
                stk.back().state = 100;
            else
                stk.back().state = 1;

            stk.emplace_back(SimRecurStack(in1, -1));
            continue;
        }
        else if( stk.back().state == 100 ) {
            dp[nowAt] = dp[in1];
            dp[nowAt] ^= (mode==9);
            dp[nowAt] ^= neg;
            stk.pop_back();
            continue;
        }
        else if( stk.back().state == 1 ) {
            stk.back().state = 2;
            stk.emplace_back(SimRecurStack(in2, -1));
            continue;
        }
        else if( stk.back().state == 2 ) {
            bool res_1 = dp[in1];
            bool res_2 = dp[in2];

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
            stk.pop_back();
        }
    }

    return dp[initAt]==1;
}
