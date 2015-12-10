#include "../catch.hpp"
#include "../../src/data_structures/recycling_vector.h"

using namespace openeda::data_structures;

TEST_CASE("data structures/recycling_vector/ empty", "[data structures][recycling_vector]") {
	recycling_vector<int, std::size_t> vec;
	REQUIRE( vec.empty() );
	REQUIRE( vec.size() == 0 );
}


TEST_CASE("data structures/recycling_vector/ insert", "[data structures][recycling_vector]") {
	recycling_vector<int, std::size_t> vec;
	vec.insert(1);
	REQUIRE( vec.size() == 1 );
	REQUIRE( !vec.empty() );
}

TEST_CASE("data structures/recycling_vector/ insert & remove", "[data structures][recycling_vector]") {
	recycling_vector<int, std::size_t> vec;
	vec.erase(vec.insert(1));
	REQUIRE( vec.empty() );
	REQUIRE( vec.size() == 0 );
}

TEST_CASE("data structures/recycling_vector/ access valid", "[data structures][recycling_vector]") {
	recycling_vector<int, std::size_t> vec;
	auto id = vec.insert(1);
	REQUIRE_NOTHROW( vec[id] );
}

TEST_CASE("data structures/recycling_vector/ access invalid", "[data structures][recycling_vector]") {
	recycling_vector<int, std::size_t> vec;
	auto id = vec.insert(1);
	REQUIRE_THROWS( vec[1999] );
}

TEST_CASE("data structures/recycling_vector/ iterating 10 elements", "[data structures][recycling_vector]") {
	recycling_vector<int, std::size_t> vec;
	std::vector<int> ten(10);
	for(auto i : ten)
		vec.insert(1);

	std::size_t i = 0;
	for(auto it = vec.begin(); it != vec.end(); ++it, ++i);
	REQUIRE( i == 10 );

}

TEST_CASE("data structures/recycling_vector/ insert 10, erase 6, iterate 4", "[data structures][recycling_vector]") {
	recycling_vector<int, std::size_t> vec;
	std::vector<std::size_t> ten(10);
	for(auto & i : ten)
		i = vec.insert(1);

	for(std::size_t i = 0; i < 10; i+=2)
		vec.erase(ten[i]);
	vec.erase(ten[9]);

	std::size_t i = 0;
	for(auto it = vec.begin(); it != vec.end(); ++it, ++i);
	REQUIRE( i == 4 );

}

TEST_CASE("data structures/recycling_vector/ insert 10, erase 10, iterate 0", "[data structures][recycling_vector]") {
	recycling_vector<int, std::size_t> vec;
	std::vector<std::size_t> ten(10);
	for(auto & i : ten)
		i = vec.insert(1);

	for(std::size_t i = 0; i < 10; ++i)
		vec.erase(ten[i]);

	std::size_t i = 0;
	for(auto it = vec.begin(); it != vec.end(); ++it, ++i);
	REQUIRE( i == 0 );

}


TEST_CASE("data structures/recycling_vector/ insert 10, erase 10, insert 1, iterate 1", "[data structures][recycling_vector]") {
	recycling_vector<int, std::size_t> vec;
	std::vector<std::size_t> ten(10);
	for(auto & i : ten)
		i = vec.insert(1);

	for(std::size_t i = 0; i < 10; ++i)
		vec.erase(ten[i]);

	vec.insert(1000);

	std::size_t i = 0;
	for(auto it = vec.begin(); it != vec.end(); ++it, ++i);
	REQUIRE( i == 1 );

}
