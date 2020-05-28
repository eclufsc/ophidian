#ifndef ICCAD2020_WRAPPER
#define ICCAD2020_WRAPPER

#include <ophidian/design/Design.h>
#include <ophidian/routing/ILPRouting.h>

namespace ophidian::routing
{
    class ICCAD2020Wrapper
    {
        public:
            using design_type               = design::Design;

            ICCAD2020Wrapper(design_type & design, std::string circuit_name);

            bool route_circuit();
        private:
            void update_ndf_constraints();
            ophidian::routing::ILPRouting        m_router;
            design_type&                         m_design;
    };
}
#endif
