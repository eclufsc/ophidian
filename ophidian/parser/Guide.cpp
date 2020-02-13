#include <fstream>
#include <regex>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "Guide.h"
#include "ParserException.h"

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
    string_type line;
    std::ifstream file (guide_file);
    if (!file.is_open()){
        throw exceptions::InexistentFile();
    }

    std::regex integer("(\\+|-)?[[:digit:]]+");
    std::regex net("n(et|_)[[:digit:]]+");
    std::regex metal("Metal[[:digit:]]+");

    std::vector<std::string> words;
    words.reserve(5);

    while (getline(file, line))
    {
        boost::split(words, line, [](char c){return c == ' ';});

        if(regex_match(words[0],net))
        { //read a net
            Guide::Net net = Guide::Net{words[0]};
            getline(file, line); //get "("
            if(line.compare("(") != 0){
                throw exceptions::GuideFileSyntaxError();
            }
            words.clear();
            while (getline(file, line) && line.compare(")") != 0)
            {
                //get the regions belonging to NET
                boost::split(words, line, [](char c){return c == ' ';});
                if(words.size() != 5){
                    throw exceptions::GuideFileSyntaxError();
                }
                if(!(regex_match(words[0], integer) && regex_match(words[1], integer) &&
                     regex_match(words[2], integer) && regex_match(words[3], integer) &&
                     regex_match(words[4], metal))  )
                {
                    throw exceptions::GuideFileSyntaxError();
                }
                int x1 = boost::lexical_cast<int>(words[0]);
                int y1 = boost::lexical_cast<int>(words[1]);
                int x2 = boost::lexical_cast<int>(words[2]);
                int y2 = boost::lexical_cast<int>(words[3]);
                auto origin = Guide::database_unit_point_type(Guide::database_unit_type(x1), Guide::database_unit_type(y1));
                auto upperRight = Guide::database_unit_point_type(Guide::database_unit_type(x2), Guide::database_unit_type(y2));
                auto region = Guide::Region(origin, upperRight, words[4]);

                net.add_region(region);
            }
            if(line.compare(")") != 0){ //get ")"
                throw exceptions::GuideFileSyntaxError();
            }
            m_nets.push_back(net);
        }
    }
    file.close();
}

const size_t Guide::size() const noexcept
{
    return m_nets.size();
}

const Guide::net_container_type& Guide::nets() const noexcept
{
    return m_nets;
}

void Guide::Net::add_region(Guide::Net::region_type &region)
{
    m_regions.push_back(region);
}

const Guide::Net::string_type& Guide::Net::name() const noexcept
{
    return m_name;
}

const Guide::Net::region_container_type& Guide::Net::regions() const noexcept
{
    return m_regions;
}

const size_t Guide::Net::regions_size() const noexcept
{
    return m_regions.size();
}

const Guide::Region::string_type& Guide::Region::metal() const noexcept
{
    return m_metal;
}

const Guide::Region::box_type &Guide::Region::region() const noexcept
{
    return m_region;
}

}
}
