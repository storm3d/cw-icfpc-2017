#include <iostream>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

json read_file(const std::string & filename)
{
    std::ifstream f;
    f.open(filename);
    json res = json::parse(f);
    f.close();
    return res;
}

void startup(int player, int players_count, const std::string & filename)
{
    json j;
    j["punter"] = player;
    j["punters"] = players_count;
    j["map"] = read_file(filename);
    std::cout<< j << std::endl;
}

json pass_move(int player)
{
  json j, j2;
  j2["punter"] = player;
  j["pass"] = j2;
  return j;
}

json claim_move(int player, int src, int dst)
{
  json j, j2;
  j2["punter"] = player;
  j2["source"] = src;
  j2["target"] = dst;
  j["claim"] = j2;
  return j;
}

int main() {
    std::cout << "Local runner for punters" << std::endl;
    //startup(0, 2);
    startup(1, 2, "../maps/sample.json");
    std::cout << pass_move(0) << claim_move(1, 2, 3) << std::endl;
    return 0;
}