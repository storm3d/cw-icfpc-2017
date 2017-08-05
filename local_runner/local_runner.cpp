#include <iostream>
#include <fstream>
#include <memory>
#include <json.hpp>

#include <getopt.h>

using json = nlohmann::json;

std::string punter_name = "punter";
std::string map_name = "sample";


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

std::string call_punter(const json& input_json)
{
    std::string cmd;
#ifdef WINVER
    cmd = "cmd /c ";
    cmd += punter_name;
    cmd += ".exe";
#else
    cmd = "./";
    cmd += punter_name;
#endif // WINDOWS
    json handshake;
    handshake["you"] = "cw";
    std::string message = wrap_json(handshake) + wrap_json(input_json);
    return exec(cmd, message);
}

json startup(int player, int players_count, const std::string & filename)
{
    json j;
    j["punter"] = player;
    j["punters"] = players_count;
    j["map"] = read_file(filename);
    return j;
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

/*json parse_response(std::stream & istream)
{
  
}*/

int main(int argc, char *argv[]) {
      int opt;
      while ((opt = getopt(argc, argv, "p:m:")) != -1) {
        switch (opt) {
        case 'p':
            punter_name = optarg;
            break;
        case 'm':
            map_name = optarg;
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-p punter] [-m map_name]\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    //
    std::cout << "Local runner for punters" << std::endl;
    std::string map = "../maps/";
    map += map_name;
    map += ".json";
    //std::cout<< map << "\n";
    json test = startup(1, 2, map);

    std::cout << call_punter(test) << std::endl;
    return 0;
}