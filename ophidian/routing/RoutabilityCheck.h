#ifndef OPHIDIAN_ROUTABILITY_CHECK_H
#define OPHIDIAN_ROUTABILITY_CHECK_H

#include <ophidian/design/Design.h>

namespace ophidian::routing
{
    bool check_connectivity(const ophidian::design::Design & design, const std::vector<ophidian::circuit::Net>& nets);

    bool pins_are_connected(const ophidian::design::Design & design, const ophidian::circuit::Net & net);
    bool segments_are_connected(const ophidian::design::Design & design, const ophidian::circuit::Net & net);

} // end namespace ophidian::routing

#endif //OPHIDIAN_ROUTABILITY_CHECK_H