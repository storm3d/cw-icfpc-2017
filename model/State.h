#ifndef CW_ICFPC_2017_STATE_H
#define CW_ICFPC_2017_STATE_H


#include <vector>


struct River {
    int from;
    int to;
};

class State {

public:
    int sites;
    std::vector<int> mines;

    std::vector<River> rivers;

    int players;

    int myId;

    int currentTurn;

};


#endif //CW_ICFPC_2017_STATE_H
