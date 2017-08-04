#ifndef CW_ICFPC_2017_STATE_H
#define CW_ICFPC_2017_STATE_H


#include <vector>
#include <iostream>

typedef unsigned short vert_t

class GameState {

public:

    GameState(std::istream &in);

    vert_t getSitesNum();
    vert_t getPlayersNum();
    vert_t getMinesNum();
    int getCurrentTurnNum();

    bool isEdge(vert_t i, vert_t j);
    bool isClaimed(vert_t i, vert_t j);
    
    int getClaimerId(vert_t i, vert_t j);

    void claimEdge(vert_t i, vert_t j);

    bool isMine(vert_t i);

    void Serialize(std::ostream &in);

protected:

    std::vector<vert_t> vertices;
    std::vector<vert_t> mines;

    int playersNum;

    int punterId;

    int currentTurn;

};


#endif //CW_ICFPC_2017_STATE_H
