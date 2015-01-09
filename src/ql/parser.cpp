
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
                        while(input != ""){
                            word = getTuple(input);
                            int rank1 = word.find("(");
                            int rank2 = word.find(")");
                            word = word.substr(rank1+1, rank2-rank1-1);
                            InsertItem ii;
                            while(true){
                                int rank = findRealMark(word, ',');
                                if(rank == -1){
                                    if(word[0] == '\''){
                                        word = word.substr(1, word.size() - 2);
                                        InsertAttrItem iai;
                                        iai.type = STRING;
                                        iai.sValue = word;
                                        ii.push_back(iai);
                                    }
                                    else{
                                        InsertAttrItem iai;
                                        iai.type = INT;
                                        iai.iValue = atoi(word.c_str());
                                        ii.push_back(iai);
                                    }
                                    break;
                                }
                                else{
                                    std::string tmp = word.substr(0, rank);
                                    if(tmp[0] == '\''){
                                        tmp = tmp.substr(1, tmp.size() - 2);
                                        InsertAttrItem iai;
                                        iai.type = STRING;
                                        iai.sValue = tmp;
                                        ii.push_back(iai);
                                    }
                                    else{
                                        InsertAttrItem iai;
                                        iai.type = INT;
                                        iai.iValue = atoi(tmp.c_str());
                                        ii.push_back(iai);
                                    }
                                    word = word.substr(rank+1, word.size() - rank - 1);
                                }
                            }
                            k->v.push_back(ii);
                        }
                    }
                    q_ = k;
                    std::cout << "insert\n";
                    std::cout << k->relName << std::endl;
                    for(int i = 0 ; i < k->v.size() ; i ++){
                        for(int j = 0 ; j < (k->v)[i].size(); j ++){
                            std::cout << (k->v)[i][j].type << std::endl;
                            if((k->v)[i][j].type == INT)
                                std::cout << (k->v)[i][j].iValue << std::endl;
                            if((k->v)[i][j].type == STRING)
                                std::cout << (k->v)[i][j].sValue << std::endl;
                        }
                    }
                }
            }

            if(firstWord == "DELETE"){
                std::string secondWord = getWord(input);
                if(secondWord == "FROM"){
                    std::string word = getWord(input);
                    DeleteQuery* k = new DeleteQuery();
                    k->relName = word;
                    k->type = Q_DELETE;
                    WhereClause wc;
                    word = getWord(input);
                    if(word == "WHERE"){
                        int rank = findRealMark(input, ' ');
                        while(rank != -1) {
                            word = input.substr(0, rank);
                            input = input.substr(rank, input.size() - rank);
                            int op;
                            if ((op = findRealMark(word, '=')) != -1) {
                                if (word[op - 1] == '!') {
                                    std::string first = word.substr(0, op - 1);
                                    std::string second = word.substr(op + 1, word.size() - op - 1);
                                    WhereItem wi;
                                    wi.op = NE_OP;
                                    wi.op1 = first;
                                    if (findRealMark(second, '.') != -1) {
                                        wi.useOp2 = 1;
                                        wi.op2 = second;
                                    }
                                    else {
                                        if (second[0] == '\'') {
                                            wi.type = STRING;
                                            wi.sValue = second.substr(1, second.size() - 2);
                                        }
                                        else {
                                            wi.type = INT;
                                            wi.iValue = atoi(second.c_str());
                                        }
                                    }
                                    wc.item.push_back(wi);
                                }
                                else {
                                    if (word[op - 1] == '<') {
                                        std::string first = word.substr(0, op - 1);
                                        std::string second = word.substr(op + 1, word.size() - op - 1);
                                        WhereItem wi;
                                        wi.op = LE_OP;
                                        wi.op1 = first;
                                        if (findRealMark(second, '.') != -1) {
                                            wi.useOp2 = 1;
                                            wi.op2 = second;
                                        }
                                        else {
                                            if (second[0] == '\'') {
                                                wi.type = STRING;
                                                wi.sValue = second.substr(1, second.size() - 2);
                                            }
                                            else {
                                                wi.type = INT;
                                                wi.iValue = atoi(second.c_str());
                                            }
                                        }
                                        wc.item.push_back(wi);
                                    }
                                    else {
                                        if (word[op - 1] == '>') {
                                            std::string first = word.substr(0, op - 1);
                                            std::string second = word.substr(op + 1, word.size() - op - 1);
                                            WhereItem wi;
                                            wi.op = GE_OP;
                                            wi.op1 = first;
                                            if (findRealMark(second, '.') != -1) {
                                                wi.useOp2 = 1;
                                                wi.op2 = second;
                                            }
                                            else {
                                                if (second[0] == '\'') {
                                                    wi.type = STRING;
                                                    wi.sValue = second.substr(1, second.size() - 2);
                                                }
                                                else {
                                                    wi.type = INT;
                                                    wi.iValue = atoi(second.c_str());
                                                }
                                            }
                                            wc.item.push_back(wi);
                                        }
                                        else {
                                            std::string first = word.substr(0, op);
                                            std::string second = word.substr(op + 1, word.size() - op - 1);
                                            WhereItem wi;
                                            wi.op = EQ_OP;
                                            wi.op1 = first;
                                            if (findRealMark(second, '.') != -1) {
                                                wi.useOp2 = 1;
                                                wi.op2 = second;
                                            }
                                            else {
                                                if (second[0] == '\'') {
                                                    wi.type = STRING;
                                                    wi.sValue = second.substr(1, second.size() - 2);
                                                }
                                                else {
                                                    wi.type = INT;
                                                    wi.iValue = atoi(second.c_str());
                                                }
                                            }
                                            wc.item.push_back(wi);
                                        }
                                    }
                                }
                            }
                            else {
                                if ((op = findRealMark(word, '>')) != -1) {
                                    std::string first = word.substr(0, op);
                                    std::string second = word.substr(op + 1, word.size() - op - 1);
                                    WhereItem wi;
                                    wi.op = GT_OP;
                                    wi.op1 = first;
                                    if (findRealMark(second, '.') != -1) {
                                        wi.useOp2 = 1;
                                        wi.op2 = second;
                                    }
                                    else {
                                        if (second[0] == '\'') {
                                            wi.type = STRING;
                                            wi.sValue = second.substr(1, second.size() - 2);
                                        }
                                        else {
                                            wi.type = INT;
                                            wi.iValue = atoi(second.c_str());
                                        }
                                    }
                                    wc.item.push_back(wi);
                                }
                                else {
                                    if ((op = findRealMark(word, '<')) != -1) {
                                        std::string first = word.substr(0, op);
                                        std::string second = word.substr(op + 1, word.size() - op - 1);
                                        WhereItem wi;
                                        wi.op = LT_OP;
                                        wi.op1 = first;
                                        if (findRealMark(second, '.') != -1) {
                                            wi.useOp2 = 1;
                                            wi.op2 = second;
                                        }
                                        else {
                                            if (second[0] == '\'') {
                                                wi.type = STRING;
                                                wi.sValue = second.substr(1, second.size() - 2);
                                            }
                                            else {
                                                wi.type = INT;
                                                wi.iValue = atoi(second.c_str());
                                            }
                                        }
                                        wc.item.push_back(wi);
                                    }
                                }
                            }
                            word = getWord(input);
                            if(word == "AND")
                                wc.op.push_back(AND_OP);
                            if(word == "OR")
                                wc.op.push_back(OR_OP);
                            rank = findRealMark(input, ' ');
                        }
                        int op;
                        word = input;
                        if ((op = findRealMark(word, '=')) != -1) {
                            if (word[op - 1] == '!') {
                                std::string first = word.substr(0, op - 1);
                                std::string second = word.substr(op + 1, word.size() - op - 1);
                                WhereItem wi;
                                wi.op = NE_OP;
                                wi.op1 = first;
                                if (findRealMark(second, '.') != -1) {
                                    wi.useOp2 = 1;
                                    wi.op2 = second;
                                }
                                else {
                                    if (second[0] == '\'') {
                                        wi.type = STRING;
                                        wi.sValue = second.substr(1, second.size() - 2);
                                    }
                                    else {
                                        wi.type = INT;
                                        wi.iValue = atoi(second.c_str());
                                    }
                                }
                                wc.item.push_back(wi);
                            }
                            else {
                                if (word[op - 1] == '<') {
                                    std::string first = word.substr(0, op - 1);
                                    std::string second = word.substr(op + 1, word.size() - op - 1);
                                    WhereItem wi;
                                    wi.op = LE_OP;
                                    wi.op1 = first;
                                    if (findRealMark(second, '.') != -1) {
                                        wi.useOp2 = 1;
                                        wi.op2 = second;
                                    }
                                    else {
                                        if (second[0] == '\'') {
                                            wi.type = STRING;
                                            wi.sValue = second.substr(1, second.size() - 2);
                                        }
                                        else {
                                            wi.type = INT;
                                            wi.iValue = atoi(second.c_str());
                                        }
                                    }
                                    wc.item.push_back(wi);
                                }
                                else {
                                    if (word[op - 1] == '>') {
                                        std::string first = word.substr(0, op - 1);
                                        std::string second = word.substr(op + 1, word.size() - op - 1);
                                        WhereItem wi;
                                        wi.op = GE_OP;
                                        wi.op1 = first;
                                        if (findRealMark(second, '.') != -1) {
                                            wi.useOp2 = 1;
                                            wi.op2 = second;
                                        }
                                        else {
                                            if (second[0] == '\'') {
                                                wi.type = STRING;
                                                wi.sValue = second.substr(1, second.size() - 2);
                                            }
                                            else {
                                                wi.type = INT;
                                                wi.iValue = atoi(second.c_str());
                                            }
                                        }
                                        wc.item.push_back(wi);
                                    }
                                    else {
                                        std::string first = word.substr(0, op);
                                        std::string second = word.substr(op + 1, word.size() - op - 1);
                                        WhereItem wi;
                                        wi.op = EQ_OP;
                                        wi.op1 = first;
                                        if (findRealMark(second, '.') != -1) {
                                            wi.useOp2 = 1;
                                            wi.op2 = second;
                                        }
                                        else {
                                            if (second[0] == '\'') {
                                                wi.type = STRING;
                                                wi.sValue = second.substr(1, second.size() - 2);
                                            }
                                            else {
                                                wi.type = INT;
                                                wi.iValue = atoi(second.c_str());
                                            }
                                        }
                                        wc.item.push_back(wi);
                                    }
                                }
                            }
                        }
                        else {
                            if ((op = findRealMark(word, '>')) != -1) {
                                std::string first = word.substr(0, op);
                                std::string second = word.substr(op + 1, word.size() - op - 1);
                                WhereItem wi;
                                wi.op = GT_OP;
                                wi.op1 = first;
                                if (findRealMark(second, '.') != -1) {
                                    wi.useOp2 = 1;
                                    wi.op2 = second;
                                }
                                else {
                                    if (second[0] == '\'') {
                                        wi.type = STRING;
                                        wi.sValue = second.substr(1, second.size() - 2);
                                    }
                                    else {
                                        wi.type = INT;
                                        wi.iValue = atoi(second.c_str());
                                    }
                                }
                                wc.item.push_back(wi);
                            }
                            else {
                                if ((op = findRealMark(word, '<')) != -1) {
                                    std::string first = word.substr(0, op);
                                    std::string second = word.substr(op + 1, word.size() - op - 1);
                                    WhereItem wi;
                                    wi.op = LT_OP;
                                    wi.op1 = first;
                                    if (findRealMark(second, '.') != -1) {
                                        wi.useOp2 = 1;
                                        wi.op2 = second;
                                    }
                                    else {
                                        if (second[0] == '\'') {
                                            wi.type = STRING;
                                            wi.sValue = second.substr(1, second.size() - 2);
                                        }
                                        else {
                                            wi.type = INT;
                                            wi.iValue = atoi(second.c_str());
                                        }
                                    }
                                    wc.item.push_back(wi);
                                }
                            }
                        }
                        k->where = wc;
                    }
                    else
                    {
                    }
                    q_ = k;
                    std::cout << "delete" << std::endl;
                    std::cout << k->relName << std::endl;
                }
            }

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

        std::string Parser::getTuple(std::string &input) {
            while(input[0] == ' '){
                input.erase(0, 1);
            }
            int rank = input.find("), (");

            if(rank != -1) {
                std::string word = input.substr(0, rank);
                input = input.substr(rank + 2, input.size() - rank - 2);
                return word;
            }
            else{
                std::string res = input;
                input = "";
                return res;
            }
        }

        int Parser::findRealMark(std::string &input, char c) {
            while(input[0] == ' '){
                input.erase(0, 1);
            }
            bool isReal = true;
            for(int i = 0 ; i < input.size() ; i ++){
                if(input[i] == c){
                    if(isReal)
                        return i;
                }
                if(input[i] == '\''){
                    isReal = !isReal;
                }
            }
            return -1;
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
