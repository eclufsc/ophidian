#define CATCH_ON true

#if CATCH_ON

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#else

#include <iostream>
#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/ILPRouting.h>
#include <ophidian/parser/ICCAD2020Writer.h>

using namespace std;
// int THREADS_DEFAULT_VALUE = 1;

void greetings(){
    using std::cout;
    using std::endl;

    cout << "=================================================" << endl;
    cout << "ICCAD 2020 CAD Contest" << endl;
    cout << "Problem B : Routing with Cell Movement" << endl;
    cout << endl;
    cout << "Team Number: cada0194:" << endl;
    cout << "Team Name: CABRA:" << endl;
    cout << "Members:" << endl;

    cout << "\tUniversity of Calgary" << endl;
    cout << "\t\tErfan Aghaeekiasaraee" << endl;
    cout << "\t\tUpma Gandhi" << endl;
    cout << "\t\tLaleh Behjat" << endl;

    cout << "\tFederal University of Santa Catarina" << endl;
    cout << "\t\tArthur Philippi Bianco" << endl;
    cout << "\t\tRenan Oliveira Netto" << endl;
    cout << "\t\tSheiny Fabre Almeida" << endl;
    cout << "\t\tTiago Augusto Fontana" << endl;
    cout << "\t\tCristina Meinhardt" << endl;
    cout << "\t\tJose Luis Guntzel" << endl;
    cout << "=================================================" << endl;
};

void run_for_circuit(ophidian::design::Design & design, std::string circuit_name, std::string output) {
    ophidian::routing::ILPRouting ilpRouting(design, circuit_name);
    ophidian::parser::ICCAD2020Writer iccad_output_writer(design, circuit_name);

    std::vector<ophidian::circuit::Net> nets(design.netlist().begin_net(), design.netlist().end_net());
    std::vector<ophidian::circuit::Net> fixed_nets;
    std::vector<ophidian::circuit::Net> routed_nets;

    std::vector<std::pair<ophidian::routing::ILPRouting::cell_type, ophidian::routing::ILPRouting::point_type>> movements; 
    // std::cout << "routing nets" << std::endl;
    auto result = ilpRouting.route_nets(nets, fixed_nets, routed_nets, movements);
    // std::cout << "result " << result << std::endl;

    if(result.first){
        iccad_output_writer.write_ICCAD_2020_output(output, movements);
    }
   
    // std::cout << "connected nets" << std::endl;
    // for (auto net : nets) {
    //     ophidian::routing::GlobalRouting::gcell_container_type pin_gcells = {};
    //     for (auto pin : design.netlist().pins(net)) {
    //         auto pin_name = design.netlist().name(pin);                
    //         auto location = design.placement().location(pin);
    //         auto box = ophidian::routing::GCellGraph::box_type{location, location};
    //         auto pin_geometry = design.placement().geometry(pin);
    //         auto layer_name = pin_geometry.front().second;
    //         auto pin_layer = design.routing_library().find_layer_instance(layer_name);
    //         auto layer_index = design.routing_library().layerIndex(pin_layer);

    //         // std::cout << "pin " << pin_name << " layer " << layer_name << " index " << layer_index << std::endl;

    //         design.global_routing().gcell_graph()->intersect(pin_gcells, box, layer_index-1);
    //     }
    //     auto connected = design.global_routing().is_connected(net, pin_gcells);

    //     auto net_name = design.netlist().name(net);
    //     if(!connected)
    //         std::cout << "net " << net_name << " is open" << std::endl;
    // }
}

bool test_input(const std::string input_file)
{
    if(input_file.find(".txt") != std::string::npos)
        return true;
    return false;
}

std::string extract_circuit_name(const std::string input_file)
{
    // Find the position of first delimiter ( last / )
    int firstDelPos = input_file.rfind("/");
    // Find the position of second delimiter
    int secondDelPos = input_file.rfind(".txt");
    auto circuit = input_file.substr(firstDelPos + 1, secondDelPos - firstDelPos - 1);
    return circuit;
}

int main(int argc, char** argv) {

    greetings();

    bool input_found{false};
    string input_file{};
    string circuit_name{};

    bool output_found{false};
    string output{};

    // HELP
    if (argc == 1 || string(argv[1]) == "-h" || string(argv[1]) == "-help" || string(argv[1]) == "--help")
    {
        cout << "usage:" << endl;
        cout << "./cell_move_router <input.txt> <output.txt>" << endl;
        return 0;
    }

    if (argc < 2)
    {
        cout << "Cannot proceed, missing inputn and/or output file name" << endl;
        cout << "For help, set --help or -help or -h" << endl;
        cout << "usage: ./cell_move_router <input.txt> <output.txt>" << endl;
        return 0;
    } 

    if (argv[1])
    {
        input_file = argv[1];
        if(test_input(input_file))
        {
            input_found = true;
            circuit_name = extract_circuit_name(input_file);
        }
    }

    if (argv[2])
    {
        output = argv[2];
        if(test_input(output))
        {
            output_found = true;
        }
    }

    // must have flags:
    if (input_file == "")
    {
        cout << "Cannot proceed, missing Input file" << endl;
        cout << "For help, set --help or -help or -h" << endl;
        return 0;
    }

    if (output == "")
    {
        cout << "Cannot proceed, missing output file name" << endl;
        cout << "For help, set --help or -help or -h" << endl;
        return 0;
    }

    auto iccad_2020 = ophidian::parser::ICCAD2020{input_file};

    auto design = ophidian::design::Design();
    ophidian::design::factory::make_design_iccad2020(design, iccad_2020);
    
    run_for_circuit(design, circuit_name, output);

    return 0;
}
#endif