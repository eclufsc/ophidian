#include "DefWriter.h"

namespace ophidian::parser
{
  //! writes replaced cells in def to the specified output path
void write_replaced_def(const ophidian::design::Design& design, const std::string input_path, const std::string output_path)
{
    using orientation = ophidian::placement::Placement::orientation_type;
    auto& netlist = design.netlist();
    auto& placement = design.placement();
    auto& std_cells = design.standard_cells();

    std::ifstream input_file(input_path);
    std::string line;
    
    std::ofstream output_file;
    output_file.open(output_path, std::ios::out|std::ios::trunc);

    bool found_components = false;
    bool once = false;
    while(std::getline(input_file, line)) 
    {
        if (found_components) {

            for(auto cell_it = netlist.begin_cell_instance(); cell_it != netlist.end_cell_instance(); cell_it++ ){
                auto cell = *cell_it;

                auto std_cell = netlist.std_cell(cell);
                output_file << "- " << netlist.name(cell) << " " << std_cells.name(std_cell);
                
                auto position = placement.location(cell);
                output_file << " + PLACED ( " << position.x().value() << " " << position.y().value() << " ) ";

                auto orientation = placement.orientation(cell);
                switch (orientation)
                {
                    case  orientation::N:
                        output_file << "N";
                        break;
                    case  orientation::S:
                        output_file << "S";
                        break;
                    case  orientation::W:
                        output_file << "W";
                        break;
                    case  orientation::E:
                        output_file << "E";
                        break;
                    case  orientation::FN:
                        output_file << "FN";
                        break;
                    case  orientation::FS:
                        output_file << "FS";
                        break;
                    case  orientation::FW:
                        output_file << "FW";
                        break;
                    case  orientation::FE:
                        output_file << "FE";
                        break;
                    default:
                        break;
                } //end switch
                output_file << std::endl << " ;" << std::endl;
            } //end for
            
            //discart the older placed components in def
            while (true)
            {
                std::getline(input_file, line);
                if (line.substr(0, 14) == "END COMPONENTS")
                {
                    output_file << line << std::endl;
                    break;
                }
            } //end while

            found_components = false;
            once == true;

        } else if (line.substr(0, 10) == "COMPONENTS" && once == false) {
            found_components = true;
            once = true;
            output_file << line << std::endl;;
        } else {
            output_file << line << std::endl;
        }
    } //end while

} // end function

} // namespace ophidian::parser