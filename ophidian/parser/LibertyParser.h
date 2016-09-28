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
    struct LUT{

    };

    struct Timing{

    };

    struct Pin{
        std::string name;
        double capacitance;
        double maxCapacitance;
        enum directionPin{ INPUT, OUTPUT};
        directionPin pinDirection;
        Timing timing;
    };

    struct Cell{
        std::string name;
        std::vector<Pin> pins;
        bool sequential = false;
    };

public:
    std::string timeUnit;
    std::string capacitiveLoadUnit;
    double capacitiveLoadUnitValue;
    std::vector<Cell> cells;

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
