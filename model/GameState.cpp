//
// Created by Dmytro Bochorishvili on 8/4/17.
//

#include "GameState.h"

#include "json.hpp"

// for convenience
using json = nlohmann::json;

GameState::GameState(std::istream &in) {
    json j;
    in >> j;
    playersNum = j["punters"];

}

vert_t GameState::getPlayersNum() {
    return playersNum;
}