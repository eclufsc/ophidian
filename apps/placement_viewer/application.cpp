#include "application.h"

#include <fstream>

#include "../../src/placement/lef.h"
#include "../../src/placement/def.h"
#include "../../src/netlist/verilog.h"

application::application(const std::string v_file, const std::string lef_file, const std::string def_file):
    m_netlist(&m_std_cells),
    m_library(&m_std_cells),
    m_placement(&m_netlist, &m_library)
{
    std::ifstream dot_lef(lef_file.c_str(), std::ifstream::in);
    std::ifstream dot_v(v_file.c_str(), std::ifstream::in);
    std::ifstream dot_def(def_file.c_str(), std::ifstream::in);
    if (!dot_def.good() || !dot_v.good() || !dot_def.good())
        throw std::runtime_error("Error reading the files");

    openeda::netlist::verilog::read(dot_v, &m_netlist);
    dot_v.close();
    openeda::placement::lef::read(dot_lef, &m_std_cells, &m_library);
    dot_lef.close();
    openeda::placement::def::read(dot_def, &m_netlist, &m_placement);
    dot_lef.close();

    for (auto c : m_netlist.cell_system()) {
        place_cell_and_update_index(c.second,
                                    m_placement.cell_position(c.second));
    }
}

application::~application() {
}


std::vector<rtree_node> application::create_rtree_nodes(
        openeda::entity::entity cell) {

    auto geometry = m_placement.cell_geometry(cell);
    std::vector<rtree_node> nodes;
    for (auto& poly : geometry) {
        point ll { std::numeric_limits<double>::max(), std::numeric_limits<
                    double>::max() };
        point ur { std::numeric_limits<double>::min(), std::numeric_limits<
                    double>::min() };
        for (auto& p : poly.outer()) {
            ll.x(std::min(ll.x(), p.x()));
            ll.y(std::min(ll.y(), p.y()));
            ur.x(std::max(ur.x(), p.x()));
            ur.y(std::max(ur.y(), p.y()));
        }
        box cellbox(ll, ur);
        rtree_node cell_node(cellbox, cell);
        nodes.push_back(cell_node);
    }
    return nodes;
}


void application::place_cell_and_update_index(openeda::entity::entity cell,
                                              point position) {
    // remove from index
    std::vector<rtree_node> nodes = create_rtree_nodes(cell);
    m_position2cellentity.remove(nodes);

    m_placement.cell_position(cell, position);

    // update index
    nodes.clear();
    nodes = create_rtree_nodes(cell);
    m_position2cellentity.insert(nodes.begin(), nodes.end());

}

openeda::entity::entity application::get_cell(point position) const
{
    std::vector<rtree_node> result;
    m_position2cellentity.query(boost::geometry::index::contains(position),
                                std::back_inserter(result));
    if(result.empty())
        return openeda::entity::entity{};
    return result.front().second;
}
