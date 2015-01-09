#include "ql/parser.h"
#include "ql/query.h"

int main() {
    std::string s;
    s = "INSERT INTO publisher VALUES (100008,'Oxbow Books Limited','PRC'), (100010,'Aardwolf Publishing','PRC'), (100012,'Miguel Angel Porrua','PRC'), (100932,'Kr??ger, Frankfurt','PRC')";
    sqleast::ql::Parser p;
    p.parse(s);

    return 0;
}