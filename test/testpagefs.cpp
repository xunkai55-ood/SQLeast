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

void interactiveTest() {
    using namespace pagefs;
    PageFS *fs = PageFS::getInstance();
    char op;
    char *c;
    int page, flag = 0;
    FileId f = fs->openFile("test3.db");
    string s;
    while (true) {
        cin >> op >> page;
        cout << "[CMD]$ ";
        switch (op) {
            case 'P':
                c = fs->loadPage(f, page);
                cout << "[P]" << c << endl;
                fs->unpinPage(f, page);
                break;
            case 'W':
                c = fs->loadPage(f, page);
                strcpy(c, "hahaha");
                fs->markDirty(f, page);
                fs->unpinPage(f, page);
                break;
            case 'S':
                fs->printState(cout);
                break;
            case 'Q':
                flag = 1;
                break;
            default:
                break;
        }
        if (flag) break;
    }
}

int main() {
//    testBuffer();
//    testSingleIO2();
//    testSingleIO3();
    interactiveTest();
    return 0;
}