#include <iostream>
#include "json.hpp"
#include "../model/GameState.h"

int main( int argc, char* argv[] ) {

    const clock_t begin_time = clock();

    // skip message prefix
    // TODO: proper reading length of JSON
    char ch;
    while (std::cin.peek() != '{')
        std::cin.get(ch);
    
    GameState state(std::cin);
    state.serialize(std::cout);

    std::cout <<  std::endl << "Execution time: "
              << float( clock () - begin_time ) / CLOCKS_PER_SEC
              << " sec" << std::endl;

    return 0;
}