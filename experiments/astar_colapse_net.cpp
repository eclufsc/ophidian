#include "astar_colapse_net.h"


AstarColapseNet::AstarColapseNet(design_type & design) : 
    m_design(design), 
    m_astar_routing(design), 
    m_global_routing(design.global_routing()),
    m_netlist(design.netlist()),
    m_placement(design.placement())
{
    std::vector<double> x_positions;
    std::vector<double> y_positions;

    for(auto net_it = m_netlist.begin_net(); net_it != m_netlist.end_net(); net_it++)
    {
        auto net = *net_it;

        x_positions.clear();
        y_positions.clear();
        for(auto pin : m_netlist.pins(net))
        {
            if(m_netlist.is_pad(pin))
            {
                auto location = m_placement.location(pin);
                x_positions.push_back(location.x().value());
                y_positions.push_back(location.y().value());
                continue;
            }
            auto cell = m_netlist.cell(pin);
            m_net_cells[net].insert(cell);

            //discovery neighbors
            for(auto pin_neighbor : m_netlist.pins(cell))
            {
                if(pin_neighbor == pin)
                    continue;
                auto net_neighbor = m_netlist.net(pin_neighbor);
                if(net_neighbor == net || net_neighbor == ophidian::circuit::Net())
                    continue;
                m_net_neighbors[net].insert(net_neighbor);
                
                //get neighbor pins
                for(auto pin : m_netlist.pins(net_neighbor))
                {
                    point_type location;
                    if(m_netlist.is_pad(pin))
                    {
                        location = m_placement.location(pin);
                    }else{
                        auto cell = m_netlist.cell(pin);
                        location = m_placement.location(cell);
                    }
                    x_positions.push_back(location.x().value());
                    y_positions.push_back(location.y().value());
                }
            }        
        }
        //calcule median of this net
        std::nth_element(x_positions.begin(), x_positions.begin() + x_positions.size()/2, x_positions.end());
        auto median_x = x_positions[x_positions.size()/2];
        std::nth_element(y_positions.begin(), y_positions.begin() + y_positions.size()/2, y_positions.end());
        auto median_y = y_positions[y_positions.size()/2];
        point_type median_point {unit_type(median_x), unit_type(median_y)};
        auto nearest_gcell = m_global_routing.gcell_graph()->nearest_gcell(median_point, 0);
        auto gcell_location = m_global_routing.gcell_graph()->center_of_box(nearest_gcell);
        m_net_median[net] = gcell_location;
    }

}

using namespace ophidian::util;

void AstarColapseNet::colapse_nets(net_container_type & nets, std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> & movements){

    int moved_nets = 0;
    int not_moved_nets = 0;
    int non_efficient_movemnt = 0;

    for(auto net : nets)
    {
        auto result = move_net(net, movements);
        /*
        return 1  -> net moved
        return 0  -> net not moved
        return -1 -> net not moved because movement is not efficient
        */
        if(result == 1){
          moved_nets++;  
        }else if (result == 0){
            not_moved_nets ++;
        }else if (result == -1){
            non_efficient_movemnt++;
        }
    }
    
    printlog();
    int sum = moved_nets + not_moved_nets + non_efficient_movemnt;
    printflog("moved net = %d -> %.2f %%\n", moved_nets, moved_nets/sum);
    printflog("NON moved net = %d -> %.2f %%\n", not_moved_nets, not_moved_nets/sum);
    printflog("movements not efficient = %d -> %.2f %%\n", non_efficient_movemnt, non_efficient_movemnt/sum);
    printflog("sum = %d\n", sum);
    printflog("Number of nets = %d\n", nets.size());
    printlog();
}

void AstarColapseNet::save_initial_state(
    const ophidian::circuit::Net & net,
    std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> & initial_cells_location,
    std::vector<ophidian::routing::AStarSegment> & initial_segments)
{
    for(auto cell : m_net_cells[net])
    {
        auto position = m_placement.location(cell);
        initial_cells_location.push_back(std::make_pair(cell, position));
    }
    for(auto segment : m_global_routing.segments(net))
    {
        initial_segments.push_back(a_star_segment_type(m_global_routing.box(segment), m_global_routing.layer_start(segment), m_global_routing.layer_end(segment), net));
    }
}

void AstarColapseNet::restore_initial_status(
    const std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> & initial_cells_location,
    const std::vector<ophidian::routing::AStarSegment> & initial_segments)
{
    for(auto pair : initial_cells_location)
    {
        m_global_routing.update_blockage_demand(m_netlist, m_placement, pair.first, true);
        m_placement.place(pair.first, pair.second);
        m_global_routing.update_blockage_demand(m_netlist, m_placement, pair.first, false);
    }
    //TODO: update extra-demand

    m_astar_routing.apply_segments_to_global_routing(initial_segments);
}

/*
return 1  -> net moved
return 0  -> net not moved
return -1 -> net not moved because movement is not efficient
*/
int AstarColapseNet::move_net(ophidian::circuit::Net & net, std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> & movements)
{
    if(m_net_neighbors[net].empty())
    {
        //movement is not possible!!
        return 0;
    }

    //initial wirelength
    scalar_type initial_wirelength = 0;
    initial_wirelength += m_global_routing.wirelength(net);
    // unroute nets
    std::vector<std::pair<cell_type, point_type>> initial_cells_location;
    std::vector<a_star_segment_type> initial_segments;
    save_initial_state(net, initial_cells_location, initial_segments);
    m_global_routing.unroute(net);
    for(auto neighbor_net : m_net_neighbors[net])
    {
        save_initial_state(neighbor_net, initial_cells_location, initial_segments);
        initial_wirelength += m_global_routing.wirelength(neighbor_net);
        m_global_routing.unroute(neighbor_net);
    }
    
    //get median of neighbor_cells
    auto median_point = m_net_median[net];

    // geting gcells arround median gcell
    auto gcells_nearest = m_global_routing.gcell_graph()->nearest_gcell(median_point, 0, 9);
    //evaluating the congestions in these gcells
    std::vector<std::pair<gcell_type, double>> gcells_ocupation;
    gcells_ocupation.reserve(gcells_nearest.size());
    for(auto gcell : gcells_nearest)
    {
        auto capacity = m_global_routing.gcell_graph()->capacity(gcell);
        auto demand = m_global_routing.gcell_graph()->demand(gcell);
        double ocupation = demand / capacity;
        gcells_ocupation.push_back(std::make_pair(gcell, ocupation));
    }
    std::sort(gcells_ocupation.begin(), gcells_ocupation.end(), [](std::pair<gcell_type, double> cost_a, std::pair<gcell_type, double> cost_b) {return cost_a.second < cost_b.second;});


    // place all cells from net in the same gcell
    for(auto cell : m_net_cells[net])
    {
        //get less congestioned gcell
        auto pair = *gcells_ocupation.begin();
        auto gcell = pair.first;
        auto ocupation = pair.second;
        gcells_ocupation.erase(gcells_ocupation.begin());

        //TODO: if ocupation is more tha X get more gcells


        auto location = m_global_routing.gcell_graph()->center_of_box(gcell);
        
        //update demand and extra demand in global routing!
        m_global_routing.update_blockage_demand(m_netlist, m_placement, cell, true);
        m_placement.place(cell, location);
        m_global_routing.update_blockage_demand(m_netlist, m_placement, cell, false);
        // TODO: update extra demand


        // WARNING : this must be improved!
        auto capacity = m_global_routing.gcell_graph()->capacity(gcell);
        auto demand = m_global_routing.gcell_graph()->demand(gcell);
        ocupation = demand / capacity;
        gcells_ocupation.push_back(std::make_pair(gcell, ocupation));
        std::sort(gcells_ocupation.begin(), gcells_ocupation.end(), [](std::pair<gcell_type, double> cost_a, std::pair<gcell_type, double> cost_b) {return cost_a.second < cost_b.second;});
    }

    //TODO: evaluate congestion in upper layers (like layer 2 and 3)


    //try to route nets
    std::vector<a_star_segment_type> segments;
    bool routed_all_nets = true;
    auto result = m_astar_routing.route_net(net, segments, false);
    if(!result)
    {
        routed_all_nets = false;
    }
    if(routed_all_nets){
        for(auto neighbor_net : m_net_neighbors[net])
        {
            auto result = m_astar_routing.route_net(neighbor_net, segments, false);
            if(!result)
            {
                routed_all_nets = false;
                break;
            }
        }
    }

    if(routed_all_nets){
        // apply changes in global routing, generate movements
        m_astar_routing.apply_segments_to_global_routing(segments);
        
        //evaluate wirelength after colapse
        scalar_type final_wirelenght = 0;
        final_wirelenght += m_global_routing.wirelength(net);
        for(auto neighbor_net : m_net_neighbors[net])
        {
            final_wirelenght += m_global_routing.wirelength(neighbor_net);
        }
        if(final_wirelenght < initial_wirelength)
        {
            // the colapse movement is not efficient!!
            restore_initial_status(initial_cells_location, initial_segments);
            return -1;
        }

        for(auto cell : m_net_cells[net])
        {  
            movements.push_back(std::make_pair(cell, m_placement.location(cell)));
        }
    }else{
        //restore initial status and return false
        restore_initial_status(initial_cells_location, initial_segments);
        return 0;
    }
    return 1;
}


