/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include "../catch.hpp"
#include "../timing-driven_placement/timingdriven_placement.h"
using namespace ophidian;
using namespace boost::units;
using namespace boost::units::si;


TEST_CASE("iterate over nets", "[tdp][iterator]")
{

    timingdriven_placement::timingdriven_placement tdp("benchmarks/simple/simple.v", "benchmarks/simple/simple.def", "benchmarks/simple/simple.lef", "benchmarks/simple/simple_Late.lib", "benchmarks/simple/simple_Early.lib", 80);

    std::cout << "circuit cells" << std::endl;
    for(timingdriven_placement::Cell cell : tdp.cells())
        std::cout << "  " << tdp.cell_name(cell) << std::endl;

    std::cout << "circuit pins" << std::endl;
    for(timingdriven_placement::Pin pin : tdp.pins())
        std::cout << "  " << tdp.pin_name(pin) << std::endl;

    std::cout << "circuit nets" << std::endl;
    for(timingdriven_placement::Net net : tdp.nets())
        std::cout << "  " << tdp.net_name(net) << std::endl;




}
