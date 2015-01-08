#ifndef QL_PARSER_H
#define QL_PARSER_H

#include "ql/query.h"
#include <string>

namespace sqleast {
    namespace ql {

        class Parser {

        public:
            StructuredQuery *parse(std::string input);

        private:
            StructuredQuery *q_;


        };

    }
}

#endif