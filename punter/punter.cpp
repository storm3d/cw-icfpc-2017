#include <iostream>
#include "json.hpp"

// for convenience
using json = nlohmann::json;

int main( int argc, char* argv[] ) {

    const clock_t begin_time = clock();

    // create an empty structure (null)
    json j;

    std::cin >> j;

    std::cout << j << std::endl;

    std::cout << "Execution time: " << float( clock () - begin_time ) / CLOCKS_PER_SEC << " sec" << std::endl;

    return 0;
}