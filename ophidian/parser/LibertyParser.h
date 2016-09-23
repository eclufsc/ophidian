#ifndef LIBERTYPARSER_H
#define LIBERTYPARSER_H

#include <memory>
#include <istream>
#include <list>
#include <map>

namespace ophidian
{
namespace parser
{

class Liberty{
public:
    std::string time_unit;
    std::string capacitive_load_unit;
    double capacitive_load_unit_value;
};

class LibertyParser
{
public:
    LibertyParser();
    ~LibertyParser();

    Liberty * readFile(const std::string & filename);
};

}
}
#endif // LIBERTYPARSER_H
