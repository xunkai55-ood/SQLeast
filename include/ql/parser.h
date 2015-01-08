#ifndef QL_PARSER_H
#define QL_PARSER_H

#include "ql/query.h"

namespace sqleast {
    namespace ql {

        class Parser {

        public:
            StructuredQuery *parse(string input);

        private:
            StructuredQuery *q_;


        };

    }
}

#endif