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
        std::vector<double> index_1;
        std::vector<double> index_2;
        std::vector<std::vector<double>> values;
    };

    struct Timing{
        LUT m_rise_delays;
        LUT m_fall_delays;
        LUT m_rise_slews;
        LUT m_fall_slews;

        LUT rise_constraint;
        LUT fall_constraint;

        enum timingType{
            combinational,
            rising_edge,
            setup_rising,
            hold_rising
        };
    };

    struct Pin{
        std::string name;
        double capacitance;
        double maxCapacitance;
        enum directionPin{ INPUT, OUTPUT};
        directionPin pinDirection;
        std::vector<Timing> timing;
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
