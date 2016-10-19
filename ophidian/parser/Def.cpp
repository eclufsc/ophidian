#include "Def.h"
#include "ParserException.h"

namespace ophidian {
namespace parser {

std::shared_ptr<Def> DefParser::readFile(const std::string & filename) const throw(InexistentFile)
{
    auto def = std::make_shared<Def>(); 
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
        Def::row r;
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

        Def::component c;
        c.name = comp->id();
        c.macro = comp->name();
        c.fixed = comp->isFixed();
        c.position = {comp->placementX(), comp->placementY()};
        c.orientation = comp->placementOrientStr();
        that.components_.push_back(c);
        return 0;
    });

    FILE* ifp = fopen(filename.c_str(), "r");
    if(ifp){
        auto res = defrRead(ifp, filename.c_str(), def.get(), true);
    } else {
        throw InexistentFile();
    }

    fclose(ifp);
    defrClear();
    
    return def;
}

DefParser::DefParser()
{ }
DefParser::~DefParser()
{ }

Def::Def()
{ }
Def::~Def()
{ }

}
}
