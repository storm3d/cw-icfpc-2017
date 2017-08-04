#include <cassert>
#include "GameState.h"
#include "json.hpp"

// for convenience
using json = nlohmann::json;

GameState::GameState(std::istream &in) {
	json j;
	in >> j;
	puntersNum = j["punters"];
    punterId = j["punter"];

	if(j["map"].is_object())
	{
		json &map = j["map"];
		if(map["sites"].is_array()) {
            incidence_list.resize(map["sites"].size());
			for (auto& element : map["sites"]) {
                int id = element["id"];

                incidence_list[id] = VertexIncidence();
				//std::cout << element << '\n';
			}
        }
    }
}

void GameState::Serialize(std::ostream &out) const {
    json j = {{"state",
                      {
                              {"punters", puntersNum}
                      }
    }};
    out << j;
}

vert_t GameState::getPuntersNum() const {
	return puntersNum;
}

punter_t GameState::getPunterId() const {
    return punterId;
}

vert_t GameState::getSitesNum() const {
	return incidence_list.size();
}

vert_t GameState::getMinesNum() const {
	return mines.size();
}

bool GameState::isMine(vert_t i) const {
	return mines.find(i) != mines.end();
}

bool GameState::isEdge(vert_t from, vert_t to) const {
	if (from > to)
	{
		vert_t tmp = from;
		from = to;
		to = tmp;
	}

	assert(to < incidence_list.size());

	auto it = incidence_list[from].find(to);
	return it != incidence_list[from].end();
}

punter_t GameState::getClaimerId(vert_t from, vert_t to) const {
	if (from > to)
	{
		vert_t tmp = from;
		from = to;
		to = tmp;
	}

	assert(to < incidence_list.size());

	auto it = incidence_list[from].find(to);
	if (it != incidence_list[from].end())
	{
		return it->second;
	}

	return 0;
}

void GameState::claimEdge(vert_t from, vert_t to, punter_t punter)  {
	if (from > to)
	{
		vert_t tmp = from;
		from = to;
		to = tmp;
	}

	assert(to < incidence_list.size());

	auto it = incidence_list[from].find(to);
	if (it != incidence_list[from].end())
	{
		incidence_list[from][to] = punter;
	}
}

const std::unordered_map<vert_t, punter_t> &GameState::getEdgesFrom(vert_t vertex) const {
	return incidence_list[vertex];
}

const std::unordered_set<vert_t>& GameState::getMines() const {
	return mines;
}
