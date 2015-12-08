#include "../catch.hpp"
#include <netlist.h>

TEST_CASE("catch hello world","[netlist][basic]"){
    
    REQUIRE( 1 == 1 );

}

TEST_CASE("catch hello world IMPORTANT","[netlist][important]"){

    REQUIRE( 2 == 2 );

}


TEST_CASE("catch hello world IMPORTANT2","[netlist][important]"){

    netlist netlist_testt;


    REQUIRE( 3 == 2 );


}

