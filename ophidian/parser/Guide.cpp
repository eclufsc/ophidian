#include "Guide.h"

namespace ophidian
{
namespace parser
{

Guide::Guide(const std::string &guide_file)
{
    read_file(guide_file);
}

void Guide::read_file(const std::string &guide_file)
{
    auto fp = std::unique_ptr<FILE, decltype( & std::fclose)>(
        std::fopen(guide_file.c_str(), "r"),
        &std::fclose);

    if(!fp) {
        throw exceptions::InexistentFile();
    }


}


void Gruide::Net::add_region(Gruide::Net::region_type &region)
{
    m_regions.push_back(region);
}

const Gruide::Net::string_type& Gruide::Net::name() const noexcept
{
    return m_name;
}

const Gruide::Net::region_container_type& Gruide::Net::regions() const noexcept
{
    return m_regions;
}



const Guide::Region::string_type& Guide::Region::metal() const noexcept
{
    return m_metal;
}

const Guide::Region::box_type &Guide::Region::region() const
{
    return m_region;
}





}
}
