#include <iostream>
#include "json.hpp"

using json = nlohmann::json;

void startup(int player, int players_count)
{
    json j;
    j["punter"] = player;
    j["punters"] = players_count;
    j["map"] = "";
    std::cout<< j << std::endl;
}

int main() {
    std::cout << "Local runner for punters" << std::endl;
    startup(0, 2);
    startup(1, 2);
    return 0;
}