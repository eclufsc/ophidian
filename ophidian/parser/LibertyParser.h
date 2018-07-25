#ifndef OPHIDIAN_PARSER_LIBERTYPARSER_H
#define OPHIDIAN_PARSER_LIBERTYPARSER_H

#include <memory>
#include <vector>
#include <string>

namespace ophidian
{
namespace parser
{
/** @brief Liberty reads a .lib file and stores its data.
 *
 * This is an encapsulation of the Liberty library made by
 * SYNOPSYS to obtain information referring to a standard cell library
 */
class Liberty{
public:
    /**
     * A structure to represent a LUT information
     */
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

        double compute(const double rv, const double cv);
    };
    /**
     * A structure to represent a Timing information
     */
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

        LUT find(LUT::lutInformation info);
    };
    /**
     * A structure to represent a pin
     */
    struct Pin{
        std::string name;
        double capacitance = 0;
        double maxCapacitance = 0;
        enum directionPin{
            INPUT,
            OUTPUT
        };
        bool clock = false;
        directionPin pinDirection;
        std::vector<Timing> timing;

        Timing find(Timing::type timingT);
    };
    /**
     * A structure to represent a cell
     */
    struct Cell{
        std::string name;
        std::vector<Pin> pins;
        bool sequential = false;

        Pin find(std::string pinName);
        std::vector<Pin> inputs();
        std::vector<Pin> outputs();
    };

    Cell find(std::string cellName);

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
#endif // OPHIDIAN_PARSER_LIBERTYPARSER_H