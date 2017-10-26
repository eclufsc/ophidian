#include "forms.h"

Form::Form()
{

}

Form::~Form()
{

}

Quad::Quad()
{

}

Quad::Quad(ophidian::circuit::Cell & cell) :
    mCell(cell)
{

}

Quad::~Quad()
{

}

bool Quad::operator==(const Quad & o) const
{
    return mCell == o.mCell;
}
