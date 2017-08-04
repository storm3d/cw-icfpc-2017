#include <cassert>
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

vert_t GameState::getSitesNum() {
	return incidence_list.size();
}

vert_t GameState::getMinesNum() {
	return mines.size();
}

bool GameState::isMine(vert_t i) {
	return mines.find(i) != mines.end();
}

bool GameState::isEdge(vert_t from, vert_t to) {
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

punter_t GameState::getClaimerId(vert_t from, vert_t to) {
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

void GameState::claimEdge(vert_t from, vert_t to, punter_t punter) {
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
