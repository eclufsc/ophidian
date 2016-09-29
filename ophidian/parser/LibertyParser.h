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

        enum lutInformation{
            RISE_DELAY,
            FALL_DELAY,
            RISE_SLEWS,
            FALL_SLEWS,
            RISE_CONSTRAINT,
            FALL_CONSTRAINT
        };

//        LUT m_rise_delays;
//        LUT m_fall_delays;
//        LUT m_rise_slews;
//        LUT m_fall_slews;

//        LUT rise_constraint;
//        LUT fall_constraint;

        type timing_type;
        unateness timing_sense;
        std::string related_pin;
        std::vector<std::pair<lutInformation, LUT>> luts;
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
