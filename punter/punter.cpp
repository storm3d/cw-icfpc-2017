#include <iostream>
#include "json.hpp"
#include "../model/GameState.h"

int main( int argc, char* argv[] ) {

    const clock_t begin_time = clock();

    GameState state(std::cin);

    std::cout << state.getPlayersNum() << std::endl;

    std::cout << "Execution time: " << float( clock () - begin_time ) / CLOCKS_PER_SEC << " sec" << std::endl;

    return 0;
}