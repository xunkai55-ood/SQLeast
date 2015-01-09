
#include "ql/query.h"
#include "ql/parser.h"

namespace sqleast {
    namespace ql {

        StructuredQuery* Parser::parse(std::string input) {
            if (q_ != nullptr) delete q_;

            std::string firstWord = getWord(input);
            if(firstWord == "CREATE"){
                std::string secondWord = getWord(input);
                if(secondWord == "DATABASE"){
                    std::string word = getWord(input);
                    SingleStringQuery* k = new SingleStringQuery();
                    strcpy(k->name, word.c_str());
                    k->type = Q_CREATE_DB;
                    q_ = k;
                    std::cout << "create db\n";
                    std::cout << k->name << std::endl;
                }
                if(secondWord == "TABLE"){
                    std::string word = getWord(input);
                    CreateTableQuery* k = new CreateTableQuery();
                    strcpy(k->name, word.c_str());
                    k->type = Q_CREATE_TABLE;
                    word = getWord(input);
                    int no = 0;
                    if(word == "("){
                        word = getWord(input);
                        while(word != ")"){
                            (k->attrs)[no].attrName = new char[MAX_NAME_LENGTH + 1];
                            strcpy((k->attrs)[no].attrName, word.c_str());
                            word = getWord(input);
                            int type, len;
                            getAttr(word, type, len);
                            (k->attrs)[no].attrLength = len;
                            (k->attrs)[no].isPrimary = false;
                            if(type == 0)
                                (k->attrs)[no].attrType = INT;
                            if(type == 1)
                                (k->attrs)[no].attrType = STRING;
                            word = getWord(input);
                            if(word == "NOT") {
                                (k->attrs)[no].nullable = false;
                                word = getWord(input);
                                word = getWord(input);
                            }
                            else{
                                (k->attrs)[no].nullable = true;
                                if(word == "PRIMARY"){
                                    word = getWord(input);
                                    if(word == "KEY"){
                                        word = getWord(input);
                                        if(word[0] == '(' && word[(int)word.size() - 1] == ')')
                                            word = word.substr(1, word.size() - 2);
                                        for(int i = 0 ; i < no ; i ++){
                                            if((k->attrs)[i].attrName == word){
                                                (k->attrs)[i].isPrimary = true;
                                            }
                                        }
                                        word = getWord(input);
                                    }
                                }
                                else{
                                }
                            }
                            no ++;
                        }
                    }
                    k->attrNum = no;
                    q_ = k;
                    std::cout << "create table\n";
                    std::cout << k->name << std::endl;
                    for(int i = 0 ; i < no ; i ++) {
                        std::cout << (k->attrs)[i].attrName << "|" << (k->attrs)[i].attrType << "|" << (k->attrs)[i].attrLength << "|"
                                << (k->attrs)[i].nullable << "|" << (k->attrs)[i].isPrimary << std::endl;
                    }
                }
            }
            if(firstWord == "DROP"){
                std::string secondWord = getWord(input);
                if(secondWord == "DATABASE"){
                    std::string word = getWord(input);
                    SingleStringQuery* k = new SingleStringQuery();
                    strcpy(k->name, word.c_str());
                    k->type = Q_DROP_DB;
                    q_ = k;
                    std::cout << "drop db\n";
                    std::cout << k->name << std::endl;
                }
                if(secondWord == "TABLE"){
                    std::string word = getWord(input);
                    SingleStringQuery* k = new SingleStringQuery();
                    strcpy(k->name, word.c_str());
                    k->type = Q_DROP_TABLE;
                    q_ = k;
                    std::cout << "drop table\n";
                    std::cout << k->name << std::endl;
                }
            }
            if(firstWord == "USE"){
                std::string word = getWord(input);
                SingleStringQuery* k = new SingleStringQuery();
                strcpy(k->name, word.c_str());
                k->type = Q_USE_DB;
                q_ = k;
                std::cout << "use database\n";
                std::cout << k->name << std::endl;
            }
            if(firstWord == "SHOW"){
                std::string secondWord = getWord(input);
                if(secondWord == "TABLES") {
                    SingleStringQuery *k = new SingleStringQuery();
                    k->type = Q_SHOW_TABLES;
                    q_ = k;
                    std::cout << "show table\n";
                }
            }
            if(firstWord == "DESC"){
                std::string word = getWord(input);
                SingleStringQuery* k = new SingleStringQuery();
                strcpy(k->name, word.c_str());
                k->type = Q_DESC_TABLE;
                q_ = k;
                std::cout << "desc table\n";
                std::cout << k->name << std::endl;
            }

            if(firstWord == "INSERT"){
                std::string secondWord = getWord(input);
                if(secondWord == "INTO"){
                    std::string word = getWord(input);
                    InsertQuery* k = new InsertQuery();
                    strcpy(k->relName, word.c_str());
                    k->type = Q_INSERT;
                    word = getWord(input);
                    if(word == "VALUES"){
                        
                    }
                }
            }

            // q_ = new CreateDBQuery();
            // q_.type = Q_CREATE_DB;
            // return q_;

            return q_;
        }

        std::string Parser::getWord(std::string& input) {
            while(input[0] == ' '){
                input.erase(0, 1);
            }
            int rank = input.find(" ");
            if(rank != -1) {
                std::string word = input.substr(0, rank);
                input = input.substr(rank + 1, input.size() - rank - 1);
                return word;
            }
            else{
                return input;
            }
        }

        void Parser::getAttr(std::string input, int& type, int& len) {
            // int 0 refer to int, while 1 refer to varchar
            int rank1 = input.find("(");
            int rank2 = input.find(")");
            std::string t = input.substr(0, rank1);
            if(t == "int")
                type = 0;
            if(t == "varchar")
                type = 1;
            std::string l = input.substr(rank1+1, rank2-rank1-1);
            len = atoi(l.c_str());
        }
    }
}
