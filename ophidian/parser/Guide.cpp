#include <fstream>
#include <regex>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "Guide.h"
#include "ParserException.h"

namespace ophidian::parser
{
    Guide::Guide(const std::string &guide_file):
        m_nets{};
    {
        read_file(guide_file);
    }

    void Guide::read_file(const std::string &guide_file)
    {
        auto line = std::string{};

        auto file = std::ifstream{guide_file};

        if (!file.is_open()){
            throw exceptions::InexistentFile{};
        }

        auto integer = std::regex{"(\\+|-)?[[:digit:]]+"};
        auto net = std::regex{"net[[:digit:]]+"};
        auto metal = std::regex{"Metal[[:digit:]]+"};

        auto words = std::vector<std::string>{};

        while (getline(file, line))
        {
            boost::split(words, line, [](char c){return c == ' ';});

            if(regex_match(words[0],net))
            {
                auto net_name = Guide::net_type::name_type{words[0]};

                getline(file, line); //get "("

                if(line.compare("(") != 0){
                    throw exceptions::GuideFileSyntaxError{};
                }

                words.clear();

                auto regions = Guide::net_type::region_container_type{};

                while (getline(file, line) && line.compare(")") != 0)
                {
                    //get the regions belonging to NET
                    boost::split(words, line, [](char c){return c == ' ';});

                    if(words.size() != 5){
                        throw exceptions::GuideFileSyntaxError{};
                    }

                    if(!(regex_match(words[0], integer) && regex_match(words[1], integer) &&
                         regex_match(words[2], integer) && regex_match(words[3], integer) &&
                         regex_match(words[4], metal))  )
                    {
                        throw exceptions::GuideFileSyntaxError{};
                    }

                    auto x1 = static_cast<double>(boost::lexical_cast<int>(words[0]));
                    auto y1 = static_cast<double>(boost::lexical_cast<int>(words[1]));
                    auto x2 = static_cast<double>(boost::lexical_cast<int>(words[2]));
                    auto y2 = static_cast<double>(boost::lexical_cast<int>(words[3]));

                    auto origin = Guide::database_unit_point_type{
                        database_unit_type{x1},
                        database_unit_type{y1}
                    };
                    auto upperRight = Guide::database_unit_point_type{
                        database_unit_type{x2},
                        database_unit_type{y2}
                    };

                    regions.emplace_back(
                        Guide::net_type::region_type::layer_name_type{words[4]},
                        Guide::net_type::region_type::geometry_type{origin, upperRight}
                    );
                }

                if(line.compare(")") != 0){ //get ")"
                    throw exceptions::GuideFileSyntaxError();
                }

                m_nets.emplace_back(
                    std::move(net_name),
                    std::move(regions)
                );
            }
        }

        file.close();
    }

    Guide::net_container_type& Guide::nets() noexcept
    {
        return m_nets;
    }

    const Guide::net_container_type& Guide::nets() const noexcept
    {
        return m_nets;
    }

    const Guide::Net::name_type& Guide::Net::name() const noexcept
    {
        return m_name;
    }

    Guide::Net::region_container_type& Guide::Net::regions() noexcept
    {
        return m_regions;
    }

    const Guide::Net::region_container_type& Guide::Net::regions() const noexcept
    {
        return m_regions;
    }

    const Guide::Region::layer_name_type& Guide::Region::metal_layer_name() const noexcept
    {
        return m_metal;
    }

    const Guide::Region::geometry_type& Guide::Region::geometry() const noexcept
    {
        return m_region;
    }
}
