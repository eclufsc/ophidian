#ifndef OPHIDIAN_PARSER_DEF_WRITER_H
#define OPHIDIAN_PARSER_DEF_WRITER_H

#include <ophidian/design/Design.h>
#include <fstream>

namespace ophidian::parser
{
  //! writes replaced cells in def to the specified output path
void write_replaced_def(const ophidian::design::Design& design, const std::string input_path, const std::string output_path);

} // namespace ophidian::parser

#endif // OPHIDIAN_PARSER_DEF_WRITER_H