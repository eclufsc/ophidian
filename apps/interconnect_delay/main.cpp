#include "../timing/spef.h"
#include "../timing/ceff.h"

#include "../standard_cell/standard_cells.h"
#include "../timing/liberty.h"

#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/prefixes.hpp>

using namespace ophidian;

using namespace boost::units;
using namespace boost::units::si;

struct calculator {

    timing::library & lib;
    entity_system::entity tarc;
    double islew;
    quantity<si::time> operator()(const quantity<capacitance> & load) const {
        return lib.timing_arc_rise_slew(tarc).compute(load, quantity<si::time>(islew*pico*seconds));
    }
};

int main(int argc, char *argv[])
{
/*
 *      1 = spef file
 *      2 = liberty file
 *      3 = driver type
 *      4 = driver input slew (ps)
 *
 *      example: file.spef file.lib INV_Z80 80
*/

    if(argc != 5)
    {
        std::cerr << "wrong args!" << std::endl;
        std::cerr << "Usage: " << argv[0] << " <.spef> <.lib> <driver type> <driver input slew in ps>" << std::endl;
        std::cerr << "Example " << argv[0] << " simple.spef simple.lib INV_Z80 80" << std::endl;
        exit(-1);
    }

    standard_cell::standard_cells std_cell;
    timing::library_timing_arcs tarcs{&std_cell};
    timing::library lib{&tarcs, &std_cell};
    timing::liberty::read(argv[2], lib);

    auto cell = std_cell.cell_create(argv[3]);

    calculator calc{lib, lib.timing_arc(std_cell.pin_create(cell, "a"), std_cell.pin_create(cell, "o")), std::stod(argv[4])};

    timing::spef input;
    input.read(argv[1]);

    std::size_t i = 0;
//    for(auto & tree : input.trees())
//    {
//        std::cout << "net " << tree.net_name << std::endl;
//        std::cout << "BEGIN" << std::endl;
//        timing::effective_capacitance_wire_model ceff;
////        ceff.simulate(calc, tree.tree.pack());
//        const interconnection::rc_tree & rc_tree = tree.tree;
//        for(lemon::ListGraph::NodeIt it(rc_tree.graph()); it != lemon::INVALID; ++it)
//            std::cout << rc_tree.capacitor_name(it) << " " << ceff.delays()[it].value() << std::endl;
//        std::cout << "END" << std::endl;
//    }

    return 0;
}
