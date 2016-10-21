#ifndef LIBERTYPARSER_H
#define LIBERTYPARSER_H

#include <memory>
#include <vector>
#include <string>

namespace ophidian
{
namespace parser
{

class Liberty{
public:
    struct LUT{
        enum lutInformation{
            CELL_FALL,
            CELL_RISE,
            FALL_TRANSITION,
            RISE_TRANSITION,
            RISE_CONSTRAINT,
            FALL_CONSTRAINT
        };

        lutInformation _lutInformation;
        std::vector<double> index_1;
        std::vector<double> index_2;
        std::vector<std::vector<double>> values;
    };

    struct Timing{
        enum unateness{
            NEGATIVE_UNATE,
            POSITIVE_UNATE,
            NON_UNATE
        };

        enum type{
            COMBINATIONAL,
            RISING_EDGE,
            SETUP_RISING,
            HOLD_RISING
        };

        type timingType;
        unateness timingSense;
        std::string relatedPin;
        std::vector< LUT > luts;
    };

    struct Pin{
        std::string name;
        double capacitance = 0;
        double maxCapacitance = 0;
        enum directionPin{
            INPUT,
            OUTPUT
        };
        directionPin pinDirection;
        std::vector<Timing> timing;
    };

    struct Cell{
        std::string name;
        std::vector<Pin> pins;
        bool sequential = false;
    };


public:
    std::string timeUnit = "";
    std::string capacitiveLoadUnit = "";
    double capacitiveLoadUnitValue = 0;
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
