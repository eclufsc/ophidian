#include <catch2/catch.hpp>

#include <ophidian/parser/ParserException.h>

namespace test {
    void throwsInexistentFile() {
        throw ophidian::parser::exceptions::InexistentFile();
    }
}

TEST_CASE("InexistentFile: Create and throw", "[parser][exception]"){
    REQUIRE_THROWS_AS(test::throwsInexistentFile(), ophidian::parser::exceptions::InexistentFile);
}
