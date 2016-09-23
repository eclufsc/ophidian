#include "Def.h"

//#include <iostream>

namespace ophidian {
namespace parser {

Def::Def(const std::string& filename)
{
    defrInit();

    defrSetUnitsCbk([](defrCallbackType_e, double number, defiUserData ud)->int{
        Def& that = *static_cast<Def*>(ud);
        that.units_ = number;
        return 0;
    });

    defrSetDieAreaCbk([](defrCallbackType_e, defiBox *box, defiUserData ud)->int{
        Def& that = *static_cast<Def*>(ud);
        that.die_.lower = {box->xl(), box->yl()};
        that.die_.upper = {box->xh(), box->yh()};
        return 0;
    });

    defrSetRowCbk([](defrCallbackType_e, defiRow *defrow, defiUserData ud)->int{
        Def& that = *static_cast<Def*>(ud);
        row r;
        r.name = defrow->name();
        r.site = defrow->macro();
        r.num = {defrow->xNum(), defrow->yNum()};
        r.step = {defrow->xStep(), defrow->yStep()};
        r.origin = {defrow->x(), defrow->y()};
        that.rows_.push_back(r);
        return 0;
    });

    defrSetComponentStartCbk([](defrCallbackType_e, int number, defiUserData ud)->int{
        Def& that = *static_cast<Def*>(ud);
        that.components_.reserve(number);
        return 0;
    });

    defrSetComponentCbk([](defrCallbackType_e, defiComponent *comp, defiUserData ud)->int{
        Def& that = *static_cast<Def*>(ud);

        component c;
        c.name = comp->id();
        c.macro = comp->name();
        c.fixed = comp->isFixed();
        c.position = {comp->placementX(), comp->placementY()};
        c.orientation = comp->placementOrientStr();
        that.components_.push_back(c);
        return 0;
    });

    FILE* ifp = fopen(filename.c_str(), "r");
    /*auto res =*/ defrRead(ifp, filename.c_str(), this, true);
    /*if(res)
        std::cerr << "DEF lib defrRead error" << std::endl;*/

    fclose(ifp);
}

Def::~Def()
{ }

}
}
