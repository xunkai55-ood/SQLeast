#ifndef CLI_SQLEAST_H
#define CLI_SQLEAST_H

#include <string>
#include <iostream>

using namespace std;

int main() {
    while (true) {
        cout << "[SQLEAST] $ " << endl;
        string s = "";
        char c;

        while ((c = getchar()) != ';') s += c;
        if (s == "QUIT") break;

        cout << "[COMMAND]:" << s << endl;
    }
    return 0;
}

#endif