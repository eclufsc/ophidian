#ifndef FORMS_H
#define FORMS_H

#include <ophidian/circuit/Netlist.h>

class Form
{
public:
    Form();
    ~Form();

    uint32_t mId;
};

class Line : public Form {
public:
    Line();
    ~Line();
};

class Quad : public Form {
public:
    Quad();
    explicit Quad(const ophidian::circuit::Cell &cell);

    ~Quad();

    bool operator==(const Quad & o) const;
    bool operator!=(const Quad & o) const;

    ophidian::circuit::Cell mCell;
};

class WireQuad {
public:
    WireQuad();
    explicit WireQuad(const ophidian::circuit::Cell &cell);

    ~WireQuad();

    std::array<Line, 4> mLines;
    ophidian::circuit::Cell mCell;
};


#endif // FORMS_H
