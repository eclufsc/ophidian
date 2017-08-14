#include "../catch.hpp"
#include "kmeans_data_oriented_design.h"
#include "kmeans_object_oriented_design.h"
#include "metrics.h"
#include "../../ophidian/parser/Def.h"

#include <memory>

// writing on a text file
#include <iostream>
#include <fstream>
#include "high_contrast_pallete_color.h"
using namespace std;


void write_svg(std::string file_name, ophidian::geometry::Point dimensions, ophidian::KmeansDataOrientedDesign &kmeansDOD){
    ofstream myfile (file_name + ".svg");
    if (myfile.is_open())
    {
        myfile << "<svg width=\"" << dimensions.x()/1000 << "\" height=\"" << dimensions.y()/1000 << "\">\n";


        high_contrast_pallete_color::initialize_palette_with_20_colors();

        unsigned cluster_index = 0;
        for (auto cluster : kmeansDOD.clusters_)
        {
            unsigned color_index = cluster_index % high_contrast_pallete_color::m_colors.size();
            auto cluster_color = high_contrast_pallete_color::m_colors.at(color_index);
            std::string color_string = "(" + boost::lexical_cast<std::string>(cluster_color.m_red) +
                                       "," + boost::lexical_cast<std::string>(cluster_color.m_green) +
                                       "," + boost::lexical_cast<std::string>(cluster_color.m_blue) + ")";

            auto cluster_center = kmeansDOD.clusterCenters_[cluster].first;

            myfile << "<line x1=\"" << ((cluster_center.x()/1000)-3) << "\" y1=\"" << ((cluster_center.y()/1000)+3) << "\" x2=\"" << ((cluster_center.x()/1000) +3) << "\" y2=\"" << ((cluster_center.y()/1000)-3) << "\" style=\"fill-opacity:1;fill:rgb" << color_string << ";stroke:rgb" << color_string << ";stroke-width:2\"/>\n";
            myfile << "<line x1=\"" << ((cluster_center.x()/1000)-3) << "\" y1=\"" << ((cluster_center.y()/1000)-3) << "\" x2=\"" << ((cluster_center.x()/1000) +3) << "\" y2=\"" << ((cluster_center.y()/1000)+3) << "\" style=\"fill-opacity:1;fill:rgb" << color_string << ";stroke:rgb" << color_string << ";stroke-width:2\"/>\n";

            for (auto flip_flop : kmeansDOD.clusterElements_[cluster])
            {
                myfile << "<circle cx=\""<< flip_flop.x()/1000 << "\" cy=\""<< flip_flop.y()/1000 << "\" r=\"2\"" << " style=\"fill-opacity:1;fill:rgb" << color_string << ";stroke:rgb" << color_string << ";stroke-width:2\"/>\n";
            }

            cluster_index++;
        }
        myfile << "</svg>";
        myfile.close();
    }
    else cout << "Unable to open file";
}


TEST_CASE("kmeans to svg in ICCAD2015","[svg]")
{
    std::vector<std::string> circuits = {
        "superblue18",
        //    "superblue4",
        //    "superblue16",
        //    "superblue5",
        //    "superblue1",
        //    "superblue3",
        //    "superblue10",
        //    "superblue7"
    };

    for(auto circuit_name : circuits)
    {
        ophidian::parser::DefParser reader;
        std::shared_ptr<ophidian::parser::Def> def = reader.readFile("./input_files/"+circuit_name+"/"+circuit_name+".def");

        std::vector<ophidian::geometry::Point> flip_flop_positions;

        int chipSlice = 2;

        auto die = (*def).die();
        ophidian::geometry::Point chipOrigin((double)die.lower.x, (double)die.lower.y);
        ophidian::geometry::Point chipBondary((double)die.upper.x/chipSlice, (double)die.upper.y/chipSlice);
        ophidian::geometry::Box chip_area(chipOrigin, chipBondary);

//        ophidian::geometry::Point chipOrigin(2061920, 0);
//        ophidian::geometry::Point chipBondary(3548480, 869100);
//        ophidian::geometry::Box chip_area(chipOrigin, chipBondary);

        std::string ff ("DFF_X80");
        for(ophidian::parser::Def::component component : def->components())
        {
            if(ff.compare(component.macro) == 0)
            {
                ophidian::geometry::Point componentPoint(component.position.x, component.position.y);
                if(boost::geometry::within(componentPoint, chip_area))
                    flip_flop_positions.push_back(componentPoint);
            }
        }

        ophidian::KmeansDataOrientedDesign kmeansDOD (chipOrigin, chipBondary, (int)(flip_flop_positions.size()/50) );
        kmeansDOD.cluster_registers_parallel(flip_flop_positions, 10);

        write_svg(circuit_name, chipBondary, kmeansDOD);
    }
}
