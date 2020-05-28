#include "ICCAD2020Wrapper.h"

namespace ophidian::routing
{
    ICCAD2020Wrapper::ICCAD2020Wrapper(design::Design & design, std::string circuit_name):
        m_design(design), m_router(design, circuit_name)
    {
    }

    bool ICCAD2020Wrapper::route_circuit()
    {
        std::vector<ophidian::circuit::Net> nets(m_design.netlist().begin_net(),
                                                 m_design.netlist().end_net());
       bool result = m_router.route_nets(nets);
       return result;
    }
}
