#include <iostream>
#include <cassert>

#include "pagefs/pagefs.h"

using namespace std;

void testSingleIO1() {
    cout << "single io 1 test begin!" << endl;
    pagefs::PageFS *fs = pagefs::PageFS::getInstance();
    fs->createFile("test.db", true);
    cout << "created!" << endl;
    pagefs::FileId f = fs->openFile("test.db");
    cout << "opened!" << endl;
    char *c = fs->loadPage(f, 0);
    cout << "loaded!" << endl;
    assert(c != nullptr);
    strcat(c, "Hello world, how are you?");
    cout << "written!" << endl;
    fs->markDirty(f, 0);
    fs->forcePage(f, 0);
    cout << "forced!" << endl;
}

void testSingleIO2() {
    cout << "single io 2 test begin!" << endl;
    pagefs::PageFS *fs = pagefs::PageFS::getInstance();
    pagefs::FileId f = fs->openFile("test.db");
    cout << "opened!" << endl;
    char *c = fs->loadPage(f, 0);
    cout << "loaded!" << endl;
    assert(c != nullptr);
    strcat(c, "This is a test.");
    cout << "written!" << endl;
    fs->markDirty(f, 0);
    fs->forcePage(f, 0);
    cout << "forced!" << endl;
}

void testBuffer() {
    pagefs::PageFS *fs = pagefs::PageFS::getInstance();
    cout << "buffer test begin!" << endl;
    pagefs::FileId f = fs->openFile("test.db");
    char *a1 = fs->loadPage(f, 0);
    fs->pinPage(f, 0);
    char *a2 = fs->loadPage(f, 1);
    fs->pinPage(f, 1);
    char *a3 = fs->loadPage(f, 2);
    fs->pinPage(f, 2);
    char *a4 = fs->loadPage(f, 3);
}

int main() {
//    testBuffer();
    testSingleIO2();
    return 0;
}