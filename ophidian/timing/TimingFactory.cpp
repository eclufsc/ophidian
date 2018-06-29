#include "TimingFactory.h"

namespace ophidian
{
namespace timing
{

factory::sta_type factory::make_static_timing_analysis(sta_type::standard_cells_type& std_cells,
                                                       sta_type::netlist_type& netlist,
                                                       sta_type::library_mapping_type& lib_mapping,
                                                       sta_type::placement_type& placement,
                                                       sta_type::placment_library_type& placement_mapping,
                                                       const sta_type::liberty_type& early,
                                                       const sta_type::liberty_type& late,
                                                       const sta_type::lef_type& lef,
                                                       const sta_type::design_constraints_type& dc) noexcept
{
    return sta_type{std_cells, netlist, lib_mapping, placement, placement_mapping, early, late, lef, dc};
}

}     // namespace TIMING
}     // namespace ophidian

#endif // OPHIDIAN_TIMING_DEF2TIMING_H
