#ifndef FORMS_H
#define FORMS_H

#include <ophidian/circuit/Netlist.h>

template<typename T>
class Form
{
public:
    Form(T & entity) :
        mEntity(entity)
    {

    }
    explicit Form()
    {

    }

    bool operator==(const Form<T> & o) const
    {
        mEntity == o.mEntity;
    }

    T mEntity;
    uint32_t mId;
};

class Quad : public Form<ophidian::circuit::Cell> {
public:
    Quad();
    explicit Quad(ophidian::circuit::Cell &cell);
};

#endif // FORMS_H
