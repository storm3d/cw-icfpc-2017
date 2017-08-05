#include <iostream>
#include <fstream>
#include <memory>
#include <json.hpp>

using json = nlohmann::json;

json read_file(const std::string & filename)
{
    std::ifstream f;
    f.open(filename);
    json res = json::parse(f);
    f.close();
    return res;
}

std::string exec(const std::string & cmd, const std::string & input) {
    std::array<char, 128> buffer;
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
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
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

int main() {
    std::cout << "Local runner for punters" << std::endl;
    json test = startup(1, 2, "../maps/sample.json");
    std::cout << pass_move(0) << claim_move(1, 2, 3) << std::endl;
    
#ifdef WINVER
    std::string cmd = "cmd /c punter.exe";
#else
    std::string cmd = "./punter";
#endif // WINDOWS

    std::cout << exec(cmd, test.dump()) << std::endl;
    return 0;
}