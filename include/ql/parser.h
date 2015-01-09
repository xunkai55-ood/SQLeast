#ifndef QL_PARSER_H
#define QL_PARSER_H

#include "ql/query.h"
#include <string>

namespace sqleast {
    namespace ql {

        class Parser {

        public:
            Parser(){
                q_ = nullptr;
            }
            StructuredQuery *parse(std::string input); // input should change \n to space and remove the end ; of input

        private:
            StructuredQuery *q_;

            std::string getWord(std::string& input);
            std::string getTuple(std::string& input);
            int findRealMark(std::string& input, char c);
            void getAttr(std::string input, int& type, int& len);
        };

    }
}

#endif