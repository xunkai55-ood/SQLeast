#include <iostream>
#include <cassert>

#include "pagefs/pagefs.h"

using namespace std;

void testSingleIO() {
    cout << "single io test begin!" << endl;
    pagefs::PageFS *fs = pagefs::PageFS::getInstance();
    fs->createFile("test.db", true);
    cout << "created!" << endl;
    pagefs::FileId f = fs->openFile("test.db");
    cout << "opened!" << endl;
    char *c = fs->loadPage(f, 0);
    cout << "loaded!" << endl;
    assert(c != nullptr);
    strcat(c, "hello world how are you");
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
    char *c = fs->loadPage(f, 3);
    cout << "loaded!" << endl;
    assert(c != nullptr);
    strcat(c, "Ha ha ha");
    cout << "written!" << endl;
    fs->markDirty(f, 3);
    fs->forcePage(f, 3);
    cout << "forced!" << endl;
}

void testSingleIO3() {
    char msg[200];
    cout << "single io 3 test begin!" << endl;
    pagefs::PageFS *fs = pagefs::PageFS::getInstance();
//    fs->createFile("test3.db", true);
    pagefs::FileId f = fs->openFile("test3.db");
    cout << "opened!" << endl;
    for (int i = 0; i < 20; i++) {
        cout << i << endl;
        char *c = fs->loadPage(f, i);
        fs->pinPage(f, i);
        sprintf(msg, "Page No.%d", i);
        cout << "loaded!" << endl;
        assert(c != nullptr);
        memset(c, 0, pagefs::PAGE_SIZE);
        strcpy(c, msg);
        cout << "written!" << endl;
        fs->markDirty(f, i);
        fs->unpinPage(f, i);
        fs->forcePage(f, i);
        cout << i << " done!" << endl;
    }
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
//    testSingleIO2();
    testSingleIO3();
    return 0;
}