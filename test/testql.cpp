#include "ql/parser.h"
#include "ql/query.h"

int main() {
    std::string s;
    s = "DELETE FROM addbc WHERE book.publisher_id>=publisher.id OR publisher.nation='PRC'";
    sqleast::ql::Parser p;
    p.parse(s);

    return 0;
}
