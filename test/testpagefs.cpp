#include <iostream>
#include <cassert>

#include "pagefs/pagefs.h"

using namespace std;

void testSingleIO() {
    cout << "single io test begin!" << endl;
    pagefs::PageFS fs;
    fs.createFile("test.db", true);
    cout << "created!" << endl;
    pagefs::FileId f = fs.openFile("test.db");
    cout << "opened!" << endl;
    char *c = fs.loadPage(f, 0);
    cout << "loaded!" << endl;
    assert(c != nullptr);
    strcat(c, "hello world how are you");
    cout << "written!" << endl;
    fs.markDirty(f, 0);
    fs.forcePage(f, 0);
    cout << "forced!" << endl;
}

void testSingleIO2() {
    cout << "single io 2 test begin!" << endl;
    pagefs::PageFS fs;
    pagefs::FileId f = fs.openFile("test.db");
    cout << "opened!" << endl;
    char *c = fs.loadPage(f, 1);
    cout << "loaded!" << endl;
    assert(c != nullptr);
    strcat(c, "I'm fine thank you and you?");
    cout << "written!" << endl;
    fs.markDirty(f, 1);
    fs.forcePage(f, 1);
    cout << "forced!" << endl;
}

int main() {
    testSingleIO2();
    return 0;
}