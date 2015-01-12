#include "frontend/parser_internal.h"
#include "ql/printer.h"

using namespace std;

Printer printer = cout;
Printer err = cerr;

int main() {

    sqleast_parse();

    printer << "bye" << endl;

    return 0;
}