#include "forms.h"

Quad::Quad()
{

}

Quad::Quad(ophidian::circuit::Cell &cell) :
    mCell(cell)
{

}

bool Quad::operator==(const Quad & o) const
{
    return mCell == o.mCell;
}
