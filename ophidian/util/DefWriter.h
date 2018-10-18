#ifndef DEFWRITER_H
#define DEFWRITER_H

#include <ophidian/floorplan/Floorplan.h>
#include <ophidian/design/Design.h>
#include <ophidian/parser/Def.h>
#include <ophidian/routing/LibraryFactory.h>
#include <string>
#include <iostream>
#include <fstream>

namespace ophidian
{
namespace util
{
  //! writes def to the specified output path
void write_def(const ophidian::design::Design& design, const std::string output_path)
{
    std::ofstream file(output_path);
    auto& floorplan = design.floorplan();
    file << "VERSION 5.8 ;"<< std::endl;
    file << "DIVIDERCHAR \"/\" ;" << std::endl;
    file << "BUSBITCHARS \"[]\" ;" << std::endl;
    file << "Design " << "something" << "_solution ;" << std::endl; //TODO pegar e printar o nome do arquivo no def?qual?
    file << "UNITS DISTANCE MICRONS 2000 ;" << std::endl; //TODO pegar e printar o Units Distance Microns

    file << std::endl;

    file << "PROPERTYDEFINITIONS" << std::endl;
    file << "   COMPONENTPIN designRuleWidth REAL ;" << std::endl;
    file << "   DESIGN FE_CORE_BOX_LL_X REAL 41.800 ;" << std::endl;
    file << "   DESIGN FE_CORE_BOX_UR_X REAL 52.200 ;" << std::endl;
    file << "   DESIGN FE_CORE_BOX_LL_Y REAL 35.910 ;" << std::endl;
    file << "   DESIGN FE_CORE_BOX_UR_Y REAL 44.460 ;" << std::endl;
    file << "END PROPERTYDEFINITIONS" << std::endl;

    file << std::endl;

    file << "DIEAREA ( " << units::unit_cast<double>(floorplan.chip_origin().x()) << " ";
    file << units::unit_cast<double>(floorplan.chip_origin().y()) << " ) ( ";
    file << units::unit_cast<double>(floorplan.chip_upper_right_corner().x()) << " ";
    file << units::unit_cast<double>(floorplan.chip_upper_right_corner().y()) << " ) ;" << std::endl;

    file << std::endl;

    int row_number = 0;
    for(auto row : floorplan.range_row()){
        file << "ROW " << "@row_name#" << row_number << " "; //TODO printar o row_name
//      file << "ROW " << floorplan.name(row) << " "; //correto

        auto site = floorplan.site(row);
        file << floorplan.name(site) << " ";

        auto origin = floorplan.origin(row);
        file << units::unit_cast<double>(origin.x()) << " " << units::unit_cast<double>(origin.y()) << " ";

        file << "@orientation "; //TODO printar a orientação

        file << "DO ?" << floorplan.number_of_sites(row) << " BY 1 ";  //TODO verificar se é vertical ou horizontal e trocar o 1 de lugar

        file << "STEP ?" << units::unit_cast<double>(floorplan.dimension(site).x()); //TODO verificar se é horizontal ou vertical, trocar onde o zero fica e se printa o y
        file << " 0";


        row_number++;
        file << std::endl;
        file << " ;" << std::endl;
    }
    file << std::endl;
    auto& library = design.routing_library();

    for (auto it = library.begin_track(); it != library.end_track(); ++it) {
        file << "TRACKS ";

        if (library.orientation(*it) == routing::TrackOrientation::X) {
            file << "X ";
        } else {
            file << "Y ";
        }
        file << units::unit_cast<double>(library.start(*it));

        file << " DO " << library.numTracks(*it);

        file << " STEP " << units::unit_cast<double>(library.space(*it));

        //file << " LAYER " << library.layer(*it);

        file << std::endl;
    }

    file.close();

}

} // namespace util
} // namespace ophidian

#endif // DEFWRITER_H
