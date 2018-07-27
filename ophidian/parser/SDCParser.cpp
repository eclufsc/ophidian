#include "SDCParser.h"

namespace ophidian
{
namespace parser
{

std::shared_ptr<DesignConstraints> SDCParser::readFile(const std::string & filename) const throw(InexistentFile)
{
    return std::make_shared<DesignConstraints>();
}

std::shared_ptr<DesignConstraints> SDCSimple::constraints() const
{
    std::shared_ptr<DesignConstraints> dc = std::make_shared<DesignConstraints>();

    dc->m_clock.name = "mclk";
    dc->m_clock.period = 80.0;
    dc->m_clock.port_name = "iccad_clk";

    DesignConstraints::input_delays_type input;
    input.delay = 0.0;
    input.port_name = "inp1";
    input.clock = "mclk";
    dc->m_input_delays.push_back(input);

    input.port_name = "inp2";
    dc->m_input_delays.push_back(input);

    DesignConstraints::driving_cell_type cell;
    cell.lib_cell = "INV_X80";
    cell.pin_name = "o";
    cell.port_name = "inp1";
    cell.slew_fall = 10.0;
    cell.slew_rise = 10.0;
    dc->m_input_drivers.push_back(cell);

    cell.port_name = "inp2";
    dc->m_input_drivers.push_back(cell);

    DesignConstraints::output_delay_type output;
    output.delay = 0.0;
    output.port_name = "out";
    output.clock = "mclk";
    dc->m_output_delays.push_back(output);

    DesignConstraints::output_load_type load;
    load.pin_load = 4.0;
    load.port_name = "out";
    dc->m_output_loads.push_back(load);

    return dc;
}

}   // namespace parser
}   // namespace ophidian
