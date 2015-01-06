#include "rm/core.h"
#include "rm/filehandle.h"
#include "rm/filescan.h"

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

int main() {
    cout << "Page size = " << pagefs::PAGE_SIZE << endl;
    testCreateFile();
    testDestroyFile();
    return 0;
}