#ifndef OPHIDIAN_INTERCONNECTION_TOEPS_H
#define OPHIDIAN_INTERCONNECTION_TOEPS_H

#include <string>

namespace ophidian {
namespace interconnection {

//! Helper class to write a .eps file
class ToEps {
public:
    //! Write .eps file
    /*!
       \param el The object one wants to draw.
       \param filename The name of the output file.
     */
    template <class T>
    static void run(const T & el, const std::string & filename);
};

}
}

#endif // OPHIDIAN_INTERCONNECTION_TOEPS_H
