#include "frontend/parser_internal.h"
#include <iostream>

void interp(NODE *n) {

    using namespace std;

    switch(n -> kind) {

        case N_CREATETABLE:            /* for CreateTable() */
            cout << "create table" << endl;
            break;

        case N_CREATEINDEX:            /* for CreateIndex() */
            cout << "create index" << endl;
            break;

        case N_DROPINDEX:            /* for DropIndex() */
            cout << "drop index" << endl;
            break;

        case N_DROPTABLE:            /* for DropTable() */
            cout << "drop table" << endl;
            break;

        case N_QUERY:            /* for Query() */
            cout << "query" << endl;
            break;

        case N_INSERT:            /* for Insert() */
            cout << "insert" << endl;
            break;

        case N_DELETE:            /* for Delete() */
            cout << "delete" << endl;
            break;

        case N_UPDATE:            /* for Update() */
            cout << "update" << endl;
            break;

        default:   // should never get here
            cout << "???" << endl;
            break;
    }
}

int main() {

    sqleast_parse();

    std::cout << "bye" << std::endl;

    return 0;
}