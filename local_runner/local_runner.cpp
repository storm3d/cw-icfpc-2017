#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <json.hpp>

#include <getopt.h>

using json = nlohmann::json;

std::vector<std::string> g_PunterNames = {};
std::string g_MapName = "sample";
int g_Punters = 0;
int g_LogPunter = 0;

json read_file(const std::string & filename)
{
    std::ifstream f;
    f.open(filename);
    json res = json::parse(f);
    f.close();
    return res;
}

std::string exec(const std::string & cmd, const std::string & input) {
    const int buf_size = 128;
    std::array<char, buf_size> buffer;
    std::string result;

    // write input to file
    // TODO: true tempfile
    auto temp = "stdin.file";
    auto f = fopen(temp, "w");
    fputs(input.c_str(),f);
    fclose(f);

    // retarget input stream to input file
    freopen(temp, "r", stdin);

    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");

    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), buf_size, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}

std::string wrap_json(const json& input)
{
    std::array<char, 12> buffer = {{}};
    std::string input_string = input.dump();
    sprintf(buffer.data(), "%u:", input_string.size());
    std::string data(buffer.data());
    return data + input_string;
}

std::string call_punter(const int punter, const json& input_json)
{
    std::string cmd;
#ifdef WINVER
    cmd = "cmd /c ";
    cmd += g_PunterNames[punter];
    cmd += ".exe";
#else
    cmd = "./";
    cmd += g_PunterNames[punter];
#endif // WINDOWS
    json handshake;
    handshake["you"] = "SERVER";
    std::string message = wrap_json(handshake) + wrap_json(input_json);
    return exec(cmd, message);
}

json create_startup(int punter, int punters, const json & map)
{
    json j;
    j["punter"] = punter;
    j["punters"] = punters;
    j["map"] = map;
    return j;
}

json create_pass_move(int punter)
{
  json j, j2;
  j2["punter"] = punter;
  j["pass"] = j2;
  return j;
}

json parse_response(const std::string & response)
{
  unsigned long len;
  char ch;
  std::istringstream in(response);
  
  in>>len>>ch;
  for(int i=len;i-->0;) in>>ch;
  in>>len>>ch;
  json j;
  j << in;
  return j;
}

struct runner_state{
  std::vector<json> m_States;
  std::vector<json> m_Moves;
  json m_Map;
  int turns;

  void init (int n, const json& map)
  {
    for (int i = 0; i < n; i++)
    {
      json j;
      j["state"] = i;
      m_States.push_back(j);
      m_Moves.push_back(create_pass_move(i));
    }
    m_Map = map;
    turns = map["rivers"].size();
  }

  void process_response(int punter, const json& data)
  {
    if (data.find("ready") == data.end())
    {
      json j;
      if (data.find("pass") != data.end())
      {
        j["pass"] = data["pass"];
      }
      else if (data.find("claim") != data.end())
      {
        j["claim"] = data["claim"];
      }
      else if (data.find("splurge") != data.end())
      {
        j["splurge"] = data["splurge"];
      }
      else
        std::cerr << "ERROR!!!" << std::endl;

      m_Moves[punter] = j;
    }
    
    m_States[punter]["state"] = data["state"];

  }

  json create_move_query(int punter)
  {
      json res;
      json moves = m_Moves;
      res["move"]["moves"] = moves;
      res["state"] = m_States[punter]["state"];
      return res;
  }

  void log(int punter, const std::string & data)
  {
    if(g_LogPunter == -1)
    {
        std::cout<<data<<std::endl;
    }
    else
    {
      if (punter == g_LogPunter)
      {
        json log_move = create_move_query(g_LogPunter);
        log_move["state"] = "###";
        std::cout<<"in:"<<std::endl<<log_move.dump()<<std::endl;
      }
    }
  }

  void first_log()
  {
    if (g_LogPunter == -1) return;

    json log_start = create_startup(g_LogPunter, g_Punters, m_Map);
    
    std::cout<<"in:"<<std::endl<<log_start.dump()<<std::endl;
  }

  // TODO: implement
  int calc_score(int punter){return 0;}

  void final_log()
  {
    if (g_LogPunter == -1) return;

    json log_stop;
    log_stop["stop"] = create_move_query(g_LogPunter)["move"];
    log_stop["state"] = "###";
    std::cout<<"in:"<<std::endl<<log_stop.dump()<<std::endl;

    std::cout<<std::endl<<"SCORES:";
    for(int i = 0; i < g_Punters; i++) std::cout<<" "<<calc_score(i)<<",";

    std::cout<<std::endl<<"OUR SCORE: " << calc_score(g_LogPunter) << std::endl;
    std::cout<<std::endl<<"FINISHED IN : "<<turns<<" TURNS"<< std::endl;
  }

} g_State;


int main(int argc, char *argv[]) {
      int opt;
      while ((opt = getopt(argc, argv, "p:m:n:l:")) != -1) {
        switch (opt) {
        case 'p':
            g_PunterNames.push_back(optarg);
            break;
        case 'm':
            g_MapName = optarg;
            break;
        case 'n':
            g_Punters = atoi(optarg);
            break;
        case 'l':
            g_LogPunter = atoi(optarg);
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-l log_punter] [-m g_MapName] [-n punters] [-p punter_name]\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // default punter name
    if (g_PunterNames.size() < 1) g_PunterNames.push_back("punter");
    // calc punters
    if (g_Punters < 2)
      g_Punters = 2;
    
    // spread values
    while (g_PunterNames.size() < g_Punters)
      g_PunterNames.push_back(g_PunterNames[g_PunterNames.size()-1]);
    
    std::cout << "Local runner for punters. log punter #" << g_LogPunter << std::endl;
    std::string map = "../maps/";
    map += g_MapName;
    map += ".json";
    json map_data = read_file(map);
    //std::cout<< map << "\n";
    
    g_State.init(g_Punters, map_data);
    g_State.first_log();

    for (int i = 0; i < g_Punters; i++)
    {
      json test = create_startup(i, g_Punters, map_data);
      std::string response = call_punter(i, test);
      g_State.log(-i, response);
      g_State.process_response(i, parse_response(response));
    }

    for(int turn = 0; turn < g_State.turns; turn++)
    {
        int punter = turn % g_Punters;
        json test = g_State.create_move_query(punter);
        std::string response = call_punter(punter, test);
        g_State.log(punter, response);
        g_State.process_response(punter, parse_response(response));
    }
    g_State.final_log();
    return 0;
}