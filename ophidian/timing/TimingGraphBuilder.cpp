#include "TimingGraphBuilder.h"

namespace ophidian
{
namespace timing
{

TimingGraphBuilder::TimingGraphBuilder()
{

}

void TimingGraphBuilder::build(circuit::Netlist & netlist,
                               standard_cell::StandardCells & stdCells,
                               const circuit::LibraryMapping & libraryMapping,
                               const TimingArcs & timingArcs,
                               const Library & timingLibrary,
                               const parser::DesignConstraints & dc,
                               TimingGraph& graph)
{
    // Creating two nodes for each pin
    for (auto pinIt = netlist.begin(circuit::Pin()); pinIt != netlist.end(circuit::Pin()); ++pinIt)
    {
        graph.rise_node_create(*pinIt);
        graph.fall_node_create(*pinIt);
    }

    // Creating net edges
    for (auto netIt = netlist.begin(circuit::Net()); netIt != netlist.end(circuit::Net()); ++netIt)
    {
        auto netPins = netlist.pins(*netIt);
        circuit::Pin source;

        for (auto pin : netPins)
        {
            standard_cell::PinDirection direction = stdCells.direction(libraryMapping.pinStdCell(pin));
            switch (direction)
            {
            case standard_cell::PinDirection::INPUT:
                break;
            case standard_cell::PinDirection::OUTPUT:
            default:
                source = pin;
                break;
            }
        }

        if (source == circuit::Pin())
            throw std::out_of_range("Graph building: source not find!");

        for (auto pin : netPins)
        {
            if (pin == source)
                continue;

            graph.arc_create(graph.rise_node(source), graph.rise_node(pin), *netIt);
            graph.arc_create(graph.fall_node(source), graph.fall_node(pin), *netIt);
        }
    }

    // Creating timing arc edges
    std::vector<circuit::Pin> inputPins;

    using std_pin_name = std::string;
    std::unordered_map<std_pin_name, circuit::Pin> outputPins;

    for (auto cellIt = netlist.begin(circuit::Cell()); cellIt != netlist.end(circuit::Cell()); ++cellIt)
    {
        auto cellPins = netlist.pins(*cellIt);
        inputPins.resize(0);
        outputPins.clear();

        for(auto pin : cellPins)
        {
            standard_cell::Pin stdPin = libraryMapping.pinStdCell(pin);
            switch(stdCells.direction(stdPin))
            {
            case standard_cell::PinDirection::INPUT:
                inputPins.push_back(pin);

                break;
            case standard_cell::PinDirection::OUTPUT:
                outputPins[stdCells.name(stdPin)] = pin;
                break;
            default:
                break;
            }
        }

        for (const auto & from : inputPins)
        {
            const auto & arcs = timingArcs.pin_arcs(libraryMapping.pinStdCell(from));

            for(const auto & arc : arcs)
            {
                auto result = outputPins.find(stdCells.name(timingArcs.to(arc)));
                if(result == outputPins.end())
                    continue;

                auto to = result->second;

                switch (timingLibrary.unateness(arc))
                {
                case Library::unateness_type::POSITIVE_UNATE:
                    graph.arc_create(graph.rise_node(from), graph.rise_node(to), arc);
                    if(timingLibrary.type(arc) != Library::timing_edge_type::RISING_EDGE)
                        graph.arc_create(graph.fall_node(from), graph.fall_node(to), arc);

                    break;

                case Library::unateness_type::NON_UNATE:
                    graph.arc_create(graph.rise_node(from), graph.rise_node(to), arc);
                    if(timingLibrary.type(arc) != Library::timing_edge_type::RISING_EDGE)
                        graph.arc_create(graph.fall_node(from), graph.fall_node(to), arc);

                    graph.arc_create(graph.rise_node(from), graph.fall_node(to), arc);
                    if(timingLibrary.type(arc) != Library::timing_edge_type::RISING_EDGE)
                        graph.arc_create(graph.fall_node(from), graph.rise_node(to), arc);

                    break;

                case Library::unateness_type::NEGATIVE_UNATE:
                default:
                    graph.arc_create(graph.rise_node(from), graph.fall_node(to), arc);
                    if(timingLibrary.type(arc) != Library::timing_edge_type::RISING_EDGE)
                        graph.arc_create(graph.fall_node(from), graph.rise_node(to), arc);

                    break;
                }
            }
        }
    }

    // Creating input drivers edges
    std::vector<TimingGraph::arc_type> outRiseArcs, outFallArcs;

    for (const auto & drivingCell : dc.m_input_drivers)
    {
        outRiseArcs.resize(0);
        outFallArcs.resize(0);
        auto PI_pin = netlist.find(circuit::Pin(), drivingCell.port_name);
        auto rise_node = graph.rise_node(PI_pin);
        auto fall_node = graph.fall_node(PI_pin);
        auto newrise_node = graph.rise_node_create(PI_pin);
        auto newfall_node = graph.fall_node_create(PI_pin);

        for (auto out = graph.out_arc_iterator(rise_node); out != lemon::INVALID; ++out)
            outRiseArcs.push_back(out);
        for (auto out = graph.out_arc_iterator(fall_node); out != lemon::INVALID; ++out)
            outFallArcs.push_back(out);

        for (auto arc : outRiseArcs)
            graph.source(arc, newrise_node);
        for (auto arc : outFallArcs)
            graph.source(arc, newfall_node);

        const auto & arcs = timingArcs.pin_arcs(stdCells.find(standard_cell::Pin(), drivingCell.lib_cell + ":" + drivingCell.pin_name));
        for (const auto & arc : arcs)
        {
            switch (timingLibrary.unateness(arc))
            {
            case Library::unateness_type::POSITIVE_UNATE:
                graph.arc_create(rise_node, newrise_node, arc);
                graph.arc_create(fall_node, newfall_node, arc);
                break;
            case Library::unateness_type::NON_UNATE:
                graph.arc_create(rise_node, newrise_node, arc);
                graph.arc_create(fall_node, newfall_node, arc);
                graph.arc_create(rise_node, newfall_node, arc);
                graph.arc_create(fall_node, newrise_node, arc);
                break;
            case Library::unateness_type::NEGATIVE_UNATE:
            default:
                graph.arc_create(rise_node, newfall_node, arc);
                graph.arc_create(fall_node, newrise_node, arc);
                break;
            }
        }
    }
}

}   // namespace timing
}   // namespace ophidian
