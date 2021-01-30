#include <fstream>
#include <regex>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "Guide.h"
#include "ParserException.h"

namespace ophidian::parser
{

Guide::Guide(const std::string &guide_file)
{
    // read_file(guide_file);
    read_comercial_file(guide_file);
}

void Guide::read_file(const std::string &guide_file)
{
    string_type line;
    std::ifstream file (guide_file);
    if (!file.is_open()){
        throw exceptions::InexistentFile();
    }

    std::regex integer("(\\+|-)?[[:digit:]]+");
    // std::regex net("n(et|_)[[:digit:]]+");
    std::regex net("(.)+"); //ispd19 benchmarks have nets with no numbers and no "net" string in the name
    std::regex metal("(M|m)etal[[:digit:]]+");

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

void Guide::read_comercial_file(const std::string &guide_file)
{
    string_type line;
    std::ifstream file (guide_file);
    if (!file.is_open()){
        throw exceptions::InexistentFile();
    }

    std::regex integer("(\\+|-)?[[:digit:]]+");
    std::regex number("^\\d+(\\.)?(\\d+)*");
    // std::regex net("n(et|_)[[:digit:]]+");
    std::regex net("(.)+"); //ispd19 benchmarks have nets with no numbers and no "net" string in the name
    std::regex metal("(M|m)etal[[:digit:]]+");
    
    std::regex initNet("(R|r)outeGuideNet");
    std::regex finalNet("(E|e)ndRouteGuideNet");
    std::regex wire("(W|w)ire");

    std::vector<std::string> words;
    words.reserve(9);

    while (getline(file, line))
    {
        boost::split(words, line, [](char c){return c == ' ';});

        if(regex_match(words[0],initNet))
        { //read a net
            Guide::Net net = Guide::Net{words[1]};
            getline(file, line); //get "("
            
            words.clear();
            boost::split(words, line, [](char c){return c == ' ';});
            // wire 97.3 35.055 97.3 35.245 V 0.5 L 3
            // words[0] = wire 
            // words[1] = x1 -> 97.3
            // words[2] = y1 -> 35.055
            // words[3] = x2 -> 97.3
            // words[4] = y2 -> 35.245
            // words[5] = V
            // words[6] = 0.5
            // words[7] = layer -> L
            // words[8] = index -> 3

            if(!regex_match(words[0],wire)){
                throw exceptions::GuideFileSyntaxError();
            }

            while ( ! regex_match(words[0],finalNet))
            {
                //get the regions belonging to NET
                if(words.size() != 9){
                    throw exceptions::GuideFileSyntaxError();
                }
                // if(!(regex_match(words[1], number) && regex_match(words[2], number) && 
                //      regex_match(words[3], number) && regex_match(words[4], number) && 
                //      regex_match(words[8], number)))
                // {
                //     throw exceptions::GuideFileSyntaxError();
                // }
                if(!(regex_match(words[1], number)))
                {
                    throw exceptions::GuideFileSyntaxError();
                }
                int x1 = static_cast<int>(boost::lexical_cast<double>(words[1])*2000);
                int y1 = static_cast<int>(boost::lexical_cast<double>(words[2])*2000);
                int x2 = static_cast<int>(boost::lexical_cast<double>(words[3])*2000);
                int y2 = static_cast<int>(boost::lexical_cast<double>(words[4])*2000);
                auto origin = Guide::database_unit_point_type(Guide::database_unit_type(x1), Guide::database_unit_type(y1));
                auto upperRight = Guide::database_unit_point_type(Guide::database_unit_type(x2), Guide::database_unit_type(y2));
                auto region = Guide::Region(origin, upperRight, "Metal"+words[8]);

                net.add_region(region);

                getline(file, line); //get next line
                words.clear();
                boost::split(words, line, [](char c){return c == ' ';});
            }
            // if(line.compare(")") != 0){ //get ")"
            //     throw exceptions::GuideFileSyntaxError();
            // }
            m_nets.push_back(net);
        }
        words.clear();
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
