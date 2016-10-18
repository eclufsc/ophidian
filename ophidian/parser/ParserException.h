#ifndef PARSER_EXCEPTION_H
#define PARSER_EXCEPTION_H

#include <exception>

namespace ophidian {
    namespace parser {

        class InexistentFile : public std::exception {
        public:
            const char* what() const noexcept override {
                return "The given file was not found";
            }
        };
    }
}

#endif
