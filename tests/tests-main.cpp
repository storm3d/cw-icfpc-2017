//
// Created by storm on 04-Aug-17.
//

//#define CATCH_CONFIG_MAIN
#include "../lib/catch.hpp"



int theAnswer() { return 42; }

TEST_CASE( "Life, the universe and everything", "[42][theAnswer]" ) {
    REQUIRE( theAnswer() == 42 );
}
