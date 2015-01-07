#include "rm/core.h"
#include "rm/filehandle.h"
#include "rm/filescan.h"
#include "sqleast.h"

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

int main() {
//    cout << "Page size = " << pagefs::PAGE_SIZE << endl;
//    testCreateFile();
//    testDestroyFile();
    testStoreRecords();
    return 0;
}