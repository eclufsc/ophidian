#ifndef EXPERIMENTS_RUN_ILP
#define EXPERIMENTS_RUN_ILP

#include <ophidian/design/Design.h>
#include <ophidian/routing/ILPRouting.h>

using movement_container_type = std::unordered_map< ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type, ophidian::entity_system::EntityBaseHash>; 

void run_mcf_multithreading(ophidian::design::Design & design);

void run_ilp_for_circuit(ophidian::design::Design & design, std::string circuit_name, movement_container_type & movements, bool initial_routing = true);

void run_circuit(ophidian::design::Design & design, std::string circuit_name);

#endif
