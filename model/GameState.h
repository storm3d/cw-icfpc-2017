#ifndef CW_ICFPC_2017_STATE_H
#define CW_ICFPC_2017_STATE_H


#include <vector>
#include <iostream>


class GameState {

public:

    GameState(std::istream &in);

    int getSitesNum();
    int getPlayersNum();
    int getMinesNum();
    int getCurrentTurnNum();

    bool isEdge(int i, int j);
    bool isClaimed(int i, int j);
    
    int getClaimerId(int i, int j);

    void claimEdge(int i, int j);

    bool isMine(int i);

    void Serialize(std::ostream &in);

protected:

    std::vector<int> vertices;
    std::vector<int> mines;

    int playersNum;

    int punterId;

    int currentTurn;

};


#endif //CW_ICFPC_2017_STATE_H
