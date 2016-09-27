#ifndef LIBERTYPARSER_H
#define LIBERTYPARSER_H

#include <memory>
#include <vector>

namespace ophidian
{
namespace parser
{

class Liberty{
public:
    struct pin{
        std::string name;
        double capacitance;
        double max_capacitance;
        enum directionPin{ INPUT, OUTPUT};
        directionPin pinDirection;
    };

    struct cell{
        std::string name;
        std::vector<pin> pins;
        bool sequential;
    };

public:
    std::string time_unit;
    std::string capacitive_load_unit;
    double capacitive_load_unit_value;
    std::vector<cell> cells;

};

class LibertyParser
{
public:
    LibertyParser();
    ~LibertyParser();

     std::shared_ptr<Liberty> readFile(const std::string & filename);
private:
     class Pimpl;
     const std::unique_ptr<Pimpl> this_;
};

}
}
#endif // LIBERTYPARSER_H
