#include "TimingGraphBuilder.h"

namespace ophidian
{
namespace timing
{

TimingGraphBuilder::TimingGraphBuilder()
{

}

std::shared_ptr<TimingGraph> TimingGraphBuilder::build(const circuit::Netlist & netlist, const standard_cell::StandardCells & stdCells, const circuit::LibraryMapping & libraryMapping, const TimingArcs & timingArcs, const Library & timingLibrary, const parser::DesignConstraints & dc)
{
    std::shared_ptr<TimingGraph> graph = std::make_shared<TimingGraph>(netlist);

    // Creating two nodes for each pin
    for (auto pinIt = netlist.begin(circuit::Pin()); pinIt != netlist.end(circuit::Pin()); ++pinIt)
    {
        graph->riseNodeCreate(*pinIt);
        graph->fallNodeCreate(*pinIt);
    }

    // Creating net edges
    for (auto netIt = netlist.begin(circuit::Net()); netIt != netlist.end(circuit::Net()); ++netIt)
    {
        auto netPins = netlist.pins(*netIt);
        circuit::Pin source;

        for (auto pin : netPins) {
            //std::string pin_name = netlist.pin_name(pin);
            standard_cell::PinDirection direction = stdCells.direction(libraryMapping.pinStdCell(pin));
            switch (direction) {
            case standard_cell::PinDirection::INPUT:
                break;
            case standard_cell::PinDirection::OUTPUT:
            default:
                source = pin;
                break;
            }
        }

        // É necessário separar esses dois for's???
        assert(!(source == circuit::Pin()));

        for (auto pin : netPins)
        {
            if (pin == source)
                continue;

            graph->arcCreate(graph->riseNode(source), graph->riseNode(pin), *netIt);
            graph->arcCreate(graph->fallNode(source), graph->fallNode(pin), *netIt);
        }
    }


    // Creating timing arc edges
    std::vector<circuit::Pin> inputPins;

    using StdPinName = std::string;
    std::unordered_map<StdPinName, circuit::Pin> outputPins;

    //    std::size_t currentCell = 0;
    //    std::vector<double> percentages {
    //        0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0
    //    };

    for (auto cellIt = netlist.begin(circuit::Cell()); cellIt != netlist.end(circuit::Cell()); ++cellIt)
    {
        //        ++currentCell;
        //        for(auto p : percentages)
        //        {
        //            if(static_cast<std::size_t>(p*netlist.cell_system().size()) == currentCell)
        //                std::cout << "       graph_builder::build(): ["<<100*p<<"%]" << std::endl << std::flush;
        //        }

        auto cellPins = netlist.pins(*cellIt);
        //        std::cout << "  graph_builder::build(): creating timing arcs for cell " << netlist.cell_name(cell.first) << std::endl << std::endl << std::flush;
        inputPins.resize(0);
        outputPins.clear();

        circuit::Pin dataPin, clkPin; // Para os testes antigos (!testCreated)???

        for(auto pin : cellPins)
        {
            standard_cell::Pin stdPin = libraryMapping.pinStdCell(pin);
            switch(stdCells.direction(stdPin))
            {
            case standard_cell::PinDirection::INPUT:
                inputPins.push_back(pin);

                if(timingLibrary.pinClock(stdPin))
                    clkPin = pin;
                else if(timingLibrary.cellSequential(libraryMapping.cellStdCell(*cellIt)))
                    dataPin = pin;

                break;
            case standard_cell::PinDirection::OUTPUT:
                outputPins[stdCells.name(stdPin)] = pin;
                break;
            default:
                break;
            }
        }

        for (auto from : inputPins)
        {
            const std::vector<TimingArc> & arcs = timingArcs.pinArcs(libraryMapping.pinStdCell(from));

            for(const auto & arc : arcs)
            {
                auto result = outputPins.find(stdCells.name(timingArcs.to(arc)));
                if(result == outputPins.end())
                    continue;

                auto to = result->second;

                switch (timingLibrary.unateness(arc))
                {
                case Library::unateness_t::POSITIVE_UNATE:
                    graph->arcCreate(graph->riseNode(from), graph->riseNode(to), arc);
                    if(timingLibrary.type(arc) != Library::timing_type_t::RISING_EDGE)
                        graph->arcCreate(graph->fallNode(from), graph->fallNode(to), arc);

                    break;

                case Library::unateness_t::NON_UNATE:
                    graph->arcCreate(graph->riseNode(from), graph->riseNode(to), arc);
                    if(timingLibrary.type(arc) != Library::timing_type_t::RISING_EDGE)
                        graph->arcCreate(graph->fallNode(from), graph->fallNode(to), arc);

                    graph->arcCreate(graph->riseNode(from), graph->fallNode(to), arc);
                    if(timingLibrary.type(arc) != Library::timing_type_t::RISING_EDGE)
                        graph->arcCreate(graph->fallNode(from), graph->riseNode(to), arc);

                    break;

                case Library::unateness_t::NEGATIVE_UNATE:
                default:
                    graph->arcCreate(graph->riseNode(from), graph->fallNode(to), arc);
                    if(timingLibrary.type(arc) != Library::timing_type_t::RISING_EDGE)
                        graph->arcCreate(graph->fallNode(from), graph->riseNode(to), arc);

                    break;
                }
            }
        }
    }

    // Creating input drivers edges
    std::vector<TimingGraph::ArcType> outRiseArcs, outFallArcs;

    for (const parser::DesignConstraints::DrivingCell drivingCell : dc.mInputDrivers)
    {
        outRiseArcs.resize(0);
        outFallArcs.resize(0);
        auto PI_pin = netlist.find(circuit::Pin(), drivingCell.portName);
        auto riseNode = graph->riseNode(PI_pin);
        auto fallNode = graph->fallNode(PI_pin);
        auto newRiseNode = graph->riseNodeCreate(PI_pin);
        auto newFallNode = graph->fallNodeCreate(PI_pin);

        // Da pra usar o metodo iterator do timinggraph???
        for (lemon::ListDigraph::OutArcIt out(graph->graph(), riseNode); out != lemon::INVALID; ++out)
            outRiseArcs.push_back(out);
        for (lemon::ListDigraph::OutArcIt out(graph->graph(), fallNode); out != lemon::INVALID; ++out)
            outFallArcs.push_back(out);

        for (auto arc : outRiseArcs)
            graph->source(arc, newRiseNode);
        for (auto arc : outFallArcs)
            graph->source(arc, newFallNode);

        //auto& arcs = lib.pin_timing_arcs(lib.pin_create(lib.cell_create(drivingCell.libCell), drivingCell.pinName));
        const std::vector<TimingArc> & arcs = timingArcs.pinArcs(stdCells.find(standard_cell::Pin(), drivingCell.libCell + ":" + drivingCell.pinName));
        for (const auto & arc : arcs)
        {
            switch (timingLibrary.unateness(arc))
            {
            case Library::unateness_t::POSITIVE_UNATE:
                graph->arcCreate(riseNode, newRiseNode, arc);
                graph->arcCreate(fallNode, newFallNode, arc);
                break;
            case Library::unateness_t::NON_UNATE:
                graph->arcCreate(riseNode, newRiseNode, arc);
                graph->arcCreate(fallNode, newFallNode, arc);
                graph->arcCreate(riseNode, newFallNode, arc);
                graph->arcCreate(fallNode, newRiseNode, arc);
                break;
            case Library::unateness_t::NEGATIVE_UNATE:
            default:
                graph->arcCreate(riseNode, newFallNode, arc);
                graph->arcCreate(fallNode, newRiseNode, arc);
                break;
            }
        }
    }

    return graph;
}

}   // namespace timing
}   // namespace ophidian
