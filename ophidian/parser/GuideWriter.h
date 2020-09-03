#ifndef OPHIDIAN_PARSER_GUIDE_WRITER_H
#define OPHIDIAN_PARSER_GUIDE_WRITER_H

#include <ophidian/design/Design.h>
#include <ophidian/parser/Guide.h>
#include <fstream>

namespace ophidian
{
namespace parser
{
    //! writes def to the specified output path
    void write_guide(const ophidian::design::Design& design, const std::string output_path);

} // namespace parser
} // namespace ophidian

#endif // OPHIDIAN_PARSER_GUIDE_WRITER_H
