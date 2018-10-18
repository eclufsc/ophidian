#include "DesignOutput.h"

namespace ophidian::design::output
{

void make_def(const design::Design& design)
{
    auto& floorplan  = design.floorplan();
    auto& std_cells  = design.standard_cells();
    auto& netlist    = design.netlist();
    auto& placement  = design.placement();
    auto& library    = design.routing_library();

     // Database_unit_box_type die_area
     auto die_area = parser::Def::database_unit_box_type {
         floorplan.chip_origin(),
         floorplan.chip_upper_right_corner()
     };
     // row_container_type rows
     auto rows = parser::Def::row_container_type{};
     for (auto row: floorplan.range_row()) {

         auto row_name = parser::Def::component_type::string_type{"ROW_NAME"};  //TODO pegar o row_name (row).name();
         auto site = floorplan.site(row);
         auto origin = floorplan.origin(row);
         auto step = floorplan.dimension(site);//step??
         auto num = floorplan.number_of_sites(row);
         auto site_name = floorplan.name(site);
         rows.emplace_back(
             std::move(row_name),
             std::move(site_name),
             std::move(origin),
             std::move(step),
             parser::Def::row_type::scalar_point_type{num,1});
     }
     // component_containter_type components
     auto components = parser::Def::component_container_type {};
     for (auto cell : netlist.range_cell()) {

         auto std_cell = netlist.std_cell(cell);
         auto name        = netlist.name(cell);
         auto macro_name  = std_cells.name(std_cell);
         auto orientation = parser::Def::component_type::Orientation{0}; //TODO pegar orientation
         auto position    = placement.location(cell);
         auto fixed       = false; // no longer supported

         components.emplace_back(
             std::move(name),
             std::move(macro_name),
             std::move(orientation),
             std::move(position),
             std::move(fixed)
         );
     }
    // scalar_type micrometer_ratio
     auto micrometer_ratio = parser::Def::scalar_type {2000};  // TODO pegar o micrometer ratio

     // track_container_type tracks
     auto tracks = parser::Def::track_container_type {};
     for (auto track: library.range_track()) {

         auto orientation = library.orientation(track);
         auto start       = library.start(track);
         auto num_tracks  = library.numTracks(track);
         auto space       = library.space(track);
         auto layer       = library.layer(track);
         auto layer_name  = library.name(layer);
         tracks.emplace_back(
             std::move(orientation),
             std::move(start),
             std::move(num_tracks),
             std::move(space),
             std::move(layer_name)
         );
     }

     // nets_container_type nets
     auto nets = parser::Def::nets_container_type {};
     for (auto net: netlist.range_net()) {

         auto net_name = netlist.name(net);
         auto net_pins = netlist.pins(net);
         parser::Def::Net::pin_container_type m_pins;

         for(auto pin : net_pins) {

             auto pin_name = netlist.name(pin);
             std::vector<parser::Def::Net::string_type> temp{};

             boost::split(temp, pin_name, boost::is_any_of(":"));

             m_pins.push_back(std::make_pair(temp.front(), temp.back()));
         }

         auto m_numPins = m_pins.size();

         nets.emplace_back(
             net_name,
             m_numPins
         );
     }

     parser::Def(die_area, rows, components, micrometer_ratio, tracks, nets);

//Def(const database_unit_box_type& die_area, const row_container_type& rows,
//const component_container_type& components, const scalar_type& micrometer_ratio,
//const track_container_type& tracks, const nets_container_type& nets);

}

}  // end namespace ophidian::design::output
