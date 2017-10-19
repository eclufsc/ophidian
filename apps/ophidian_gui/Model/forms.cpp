#include "forms.h"

Quad::Quad()
{

}

bool Quad::operator==(const Quad & o) const
{
    return mCell == o.mCell;
}
