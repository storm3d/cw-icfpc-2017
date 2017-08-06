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
    turns = 1;
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

  json m_Move;
  void finish_process()
  {
    json moves = m_Moves;
    m_Move["move"]["moves"] = moves;
  }

  json create_move_query(int punter)
  {
    m_Move["state"] = m_States[punter];
  }

} g_State;

int main(int argc, char *argv[]) {
      int opt;
      while ((opt = getopt(argc, argv, "p:m:n:")) != -1) {
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
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-m g_MapName] [-n punters] [-p punter_name]\n",
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
    
    std::cout << "Local runner for punters" << std::endl;
    std::string map = "../maps/";
    map += g_MapName;
    map += ".json";
    json map_data = read_file(map);
    //std::cout<< map << "\n";
    
    g_State.init(g_Punters, map_data);
    
    for (int i = 0; i < g_Punters; i++)
    {
      json test = create_startup(i, g_Punters, map_data);
      std::string response = call_punter(i, test);
        std::cout<<response<<std::endl;
      g_State.process_response(i, parse_response(response));
    }
    g_State.finish_process();

    //std::cout<<g_State.m_Move.dump(1)<<std::endl;
    
    for(int turn = 0; turn < g_State.turns; turn++)
    {
      for (int i = 0; i < g_Punters; i++)
      {
        json test = g_State.create_move_query(i);
        std::string response = call_punter(i, test);
        g_State.process_response(i, parse_response(response));
      }
      g_State.finish_process();
    }
    return 0;
}