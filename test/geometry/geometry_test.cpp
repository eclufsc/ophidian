#include "../catch.hpp"

#include "../geometry/geometry.h"
#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/algorithms/envelope.hpp>

TEST_CASE("geometry/distance between points", "[geometry][distance]")
{

	openeda::geometry::point<double> point1(0.0, 0.0);
	openeda::geometry::point<double> point2(std::sqrt(2.0), std::sqrt(2.0));

	auto distance = boost::geometry::distance(point1, point2);
	REQUIRE( distance == 2.0 );

}
