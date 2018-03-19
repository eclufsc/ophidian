#include "SDCParser.h"

namespace ophidian
{
namespace parser
{

DesignConstraints::DesignConstraints()
{
    
}

DesignConstraints::~DesignConstraints()
{
    
}

SDCParser::SDCParser()
{
    
}

SDCParser::~SDCParser()
{
    
}

std::shared_ptr<DesignConstraints> SDCParser::readFile(const std::string & filename) const throw(InexistentFile)
{
    return std::make_shared<DesignConstraints>();
}

SDCSimple::SDCSimple()
{

}

std::shared_ptr<DesignConstraints> SDCSimple::constraints() const
{
    std::shared_ptr<DesignConstraints> dc = std::make_shared<DesignConstraints>();

    dc->mClock.name = "mclk";
    dc->mClock.period = 80.0;
    dc->mClock.portName = "iccad_clk";

    DesignConstraints::InputDelays input;
    input.delay = 0.0;
    input.portName = "inp1";
    input.clock = "mclk";
    dc->mInputDelays.push_back(input);

    input.portName = "inp2";
    dc->mInputDelays.push_back(input);

    DesignConstraints::DrivingCell cell;
    cell.libCell = "INV_X80";
    cell.pinName = "o";
    cell.portName = "inp1";
    cell.slewFall = 10.0;
    cell.slewRise = 10.0;
    dc->mInputDrivers.push_back(cell);

    cell.portName = "inp2";
    dc->mInputDrivers.push_back(cell);

    DesignConstraints::OutputDelay output;
    output.delay = 0.0;
    output.portName = "out";
    output.clock = "mclk";
    dc->mOutputDelays.push_back(output);

    DesignConstraints::OutputLoad load;
    load.pinLoad = 4.0;
    load.portName = "out";
    dc->mOutputLoads.push_back(load);

    return dc;
}

}   // namespace parser
}   // namespace ophidian
