#include "ql/parser.h"
#include "ql/query.h"

int main() {
    std::string s;
    s = "CREATE TABLE book ( id int(10) NOT NULL, title varchar(100) NOT NULL, authors varchar(200), publisher_id int(10) NOT NULL, copies int(10), PRIMARY KEY (id) )";
    sqleast::ql::Parser p;
    p.parse(s);

    return 0;
}