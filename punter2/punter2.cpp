
#include <iostream>
#include "json.hpp"

using json = nlohmann::json;

static std::vector<std::vector<int>> rivers;
static std::map<int, int> ids;
static int riversN = 0;

void get_rivers(json& map)
{
  if (map["rivers"].is_array()) {
        }
}

int main() {
    // send stub handshake
    // TODO: proper handshake
    nlohmann::json j0;
    j0["me"] = "cw0";
    std::string handshake = j0.dump();
    std::cout<<handshake.size()<<":"<<handshake;

    const clock_t begin_time = clock();

    // skip message prefix
    // TODO: proper reading length of JSON
    char ch;
    unsigned long len;
    std::cin>>len>>ch;
    for(int i=len; i-->0;) std::cin>>ch; // skip handshake
    std::cin>>len>>ch;
    
    nlohmann::json j, jres;
    j<<std::cin;

    if (j.find("move") == j.end())
    {
        jres["ready"] = j["punter"];
        jres["state"] = j;
    }
    else
    {
        for (auto &element : j["move"]["moves"])
        {
            if (element.find("pass") == element.end())
            {
                int source = element["claim"]["source"];
                int target = element["claim"]["target"];
                int idx = 0;
                for (auto &element_r : j["state"]["map"]["rivers"])
                {
                    int source_r = element_r["source"];
                    int target_r = element_r["target"];
                    if ((source_r == source && target_r == target) || (source_r == target && target_r == source))
                    {
                        j["state"]["map"]["rivers"].erase(idx);
                        break;
                    }
                    ++idx;
                }

            }
        }
        //jres["punter"] = j["state"]["punter"];
        json move;
        move["punter"] = j["state"]["punter"];
        json r = j["state"]["map"]["rivers"];
        //
        move["source"] = r.at(0)["source"];
        move["target"] = r.at(0)["target"];

        jres["claim"] = move;
        jres["state"] = j["state"];
    }

    std::string sres = jres.dump();
    std::cout<<sres.size()<<":"<<sres;
}