#ifndef EXPERIMENTS_RUN_ILP
#define EXPERIMENTS_RUN_ILP

#include <ophidian/design/Design.h>
#include <ophidian/routing/ILPRouting.h>

void run_mcf_multithreading(ophidian::design::Design & design);

void run_ilp_for_circuit(ophidian::design::Design & design, std::string circuit_name, std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements, bool initial_routing = true);

void run_circuit(ophidian::design::Design & design, std::string circuit_name);

#endif
