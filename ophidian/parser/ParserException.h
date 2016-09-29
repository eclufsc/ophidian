#ifndef PARSER_EXCEPTION_H
#define PARSER_EXCEPTION_H

#include <string>

namespace ophidian {
    namespace parser {

        class InexistentFile {
            std::string errorMessage(){
                return "The given file was not found";
        };

    }
}

#endif
