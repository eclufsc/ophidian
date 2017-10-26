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

class Quad : public Form {
public:
    Quad();
    explicit Quad(ophidian::circuit::Cell &cell);

    ~Quad();

    bool operator==(const Quad & o) const;

    ophidian::circuit::Cell mCell;
};

#endif // FORMS_H
