#include "rm/core.h"
#include "rm/filehandle.h"
#include "rm/filescan.h"

using namespace sqleast;
using namespace sqleast::rm;
using namespace std;

void testCreateFile() {
    int n;
    cout << "Record size = ";
    cin >> n;
    RecordManager::createFile("rmtest.db", n, true);
    FileHandle f = RecordManager::openFile("rmtest.db");
    cout << "File created with record size " << n << endl;
    FileInfo info = f.getInfo();
    cout << "Slot per page = " << info.slotPerPage << endl;
    cout << "Slot bitmap size = " << info.slotBitmapSize << endl;
    cout << "Total page num = " << info.totalPageNum << endl;
}

void testDestroyFile() {
    RecordManager::destroyFile("rmtest.db");
}

void testStoreRecords() {
    int flagLen = sizeof(int);
    int dataLen = 32;
    int bitmapLen = 0;
    int rLen = flagLen + dataLen + bitmapLen;
    RecordManager::createFile("rmtest.db", rLen, true);
    FileHandle f = RecordManager::openFile("rmtest.db");
    Record r(rLen);
    for (int i = 0; i < 20; i++) {
        memset(r.rData, 0, r.size);
        sprintf(r.rData + sizeof(int), "fuck you baby no %d", i);
        f.insertRec(r);
    }
}

void interativeTest() {

    struct myData {
        int a, b;
        char c[10];
    };
    string cmd;
    size_t rLen = FLAG_SIZE + sizeof(myData) + 1;
    cout << "BOUND" << endl;
    RecordManager::createFile("rmtest2.db", rLen, true);
    FileHandle f = RecordManager::openFile("rmtest2.db");
    cout << "[INTERACTIVE TEST]" << endl;
    Record r(rLen);
    int flag;
    string c;
    while (true) {
        cin >> cmd;
        flag = 0;
        switch (cmd[0]) {
            case 'Q': flag = 1; break;
            case 'I':
                int a, b;
                cin >> a >> b >> c;
                myData d;
                d.a = a;
                d.b = b;
                strcpy(d.c, c.c_str());
                memcpy(r.getData(), &d, sizeof(d));
                f.insertRec(r);
                cout << r.rid.pageNum << " " << r.rid.slotNum << endl;
                break;
            case 'D':
                int pageNum, slotNum;
                cin >> pageNum >> slotNum;
                f.deleteRec(RID(pageNum, slotNum));
            default:
                cout << "UNKNOWN COMMAND" << endl;
        }
        if (flag) break;
    }
}

int main() {
//    cout << "Page size = " << pagefs::PAGE_SIZE << endl;
//    testCreateFile();
//    testDestroyFile();
//    testStoreRecords();
    interativeTest();
    return 0;
}