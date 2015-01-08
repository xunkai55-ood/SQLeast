#include "ql/query.h"
#include "ql/parser.h"

namespace sqleast {
    namespace ql {

        StructuredQuery* Parser::parse(std::string input) {
            if (q_ != nullptr) delete q_;

            
            // q_ = new CreateDBQuery();
            // q_.type = Q_CREATE_DB;
            // return q_;

        }
    }
}
