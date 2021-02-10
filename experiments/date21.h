#ifndef EXPERIMENTS_DATE21
#define EXPERIMENTS_DATE21

#include <ophidian/design/Design.h>
#include <ophidian/routing/Engine.h>

using movement_container_type = std::unordered_map< ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type, ophidian::entity_system::EntityBaseHash>; 


    // "exp1" --> "Astar_without_paneling_and_without_movements"
    // "exp2" --> "Astar_with_paneling_and_without_movements"
    // "exp3" --> "Astar_with_paneling_and_with_movements"
    // "exp4" --> "Astar_with_paneling_and_with_movements_parallel"

    // "exp5" --> "ILP_without_movements_Astar_without_movements"
    // "exp6" --> "ILP_with_movements_Astar_with_movements"
    // "exp7" --> "ILP_with_movements_Astar_with_movements_parallel"

    void write_csv_header(std::string csv_file_name);
    void write_csv(ophidian::design::Design & design, std::string circuit_name, std::string csv_file_name, double runtime, unsigned number_of_movements);
    bool check_nets_connectivity(const ophidian::design::Design & design, const std::vector<ophidian::circuit::Net>& nets);

    void Astar_without_paneling_and_without_movements(ophidian::design::Design & design, std::string circuit_name, std::string output);
    void Astar_with_paneling_and_without_movements(ophidian::design::Design & design, std::string circuit_name, std::string output);
    void Astar_with_paneling_and_with_movements(ophidian::design::Design & design, std::string circuit_name, std::string output);
    void Astar_with_paneling_and_with_movements_parallel(ophidian::design::Design & design, std::string circuit_name, std::string output);
    
    void ILP_without_movements_Astar_without_movements(ophidian::design::Design & design, std::string circuit_name, std::string output, movement_container_type & movements);
    void ILP_with_movements_Astar_with_movements(ophidian::design::Design & design, std::string circuit_name, std::string output, movement_container_type & movements);
    void ILP_with_movements_Astar_with_movements_parallel(ophidian::design::Design & design, std::string circuit_name, std::string output, movement_container_type & movements, UCal::Engine & engine);

    void draw_gcell_svg(ophidian::design::Design & design, std::string net_name);
#endif