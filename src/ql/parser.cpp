#include "ql/query.h"
#include "ql/parser.h"

namespace sqleast {
    namespace ql {

        StructuredQuery* Parser::parse(std::string input) {
            if (q_ != nullptr) delete q_;

            std::string firstWord = getWord(input);
            switch(firstWord){
                case "CREATE":
                {
                    std::string secondWord = getWord(input);
                    if(secondWord == "DATABASE"){
                        q_ = new SingleStringQuery();
                        q_.type = Q_CREATE_DB;
                    }
                    if(secondWord == "TABLE"){
                        q_ = new SingleStringQuery;
                        q_.type = Q_CREATE_TABLE;
                    }
                }
                    break;
                default:
                    break;
            }

            // q_ = new CreateDBQuery();
            // q_.type = Q_CREATE_DB;
            // return q_;

            return q_;
        }

        std::string Parser::getWord(std::string& input) {
            int rank = input.find(" ");
            std::string word = input.substr(0, rank);
            input = input.substr(rank+1, input.size() - rank - 1);
            return word;
        }
    }
}
