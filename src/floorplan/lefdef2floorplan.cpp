#include "lefdef2floorplan.h"

namespace ophidian {
namespace floorplan {


void lefdef2floorplan(const parsing::lef &lef, const parsing::def &def, floorplan &fplan)
{
    fplan.chip_origin({def.die().lower.x, def.die().lower.y});
    fplan.chip_boundaries({def.die().upper.x, def.die().upper.y});
    for(const parsing::lef::site & site : lef.sites())
        fplan.site_insert(site.name, {site.x, site.y});
    for(const parsing::def::row & row : def.rows())
        fplan.row_insert(row.site, row.num.x, {row.origin.x, row.origin.y});
}

}
}

