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

Quad::Quad(const ophidian::circuit::Cell & cell) :
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

bool Quad::operator!=(const Quad & o) const
{
    return !(mCell == o.mCell);
}

Line::Line()
{

}

Line::~Line()
{

}

WireQuad::WireQuad()
{

}

WireQuad::WireQuad(const ophidian::circuit::Cell &cell) :
    mCell(cell)
{

}

WireQuad::~WireQuad()
{

}
