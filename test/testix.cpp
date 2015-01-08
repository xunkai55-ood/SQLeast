#include <iostream>
#include "ix/index.h"
#include "ix/indexscan.h"

using namespace std;

int page = 9;
int slot = 5;

void generateRID(sqleast::RID &rid){
    rid.pageNum = page++;
    rid.slotNum = slot++;
}

int main()
{
    sqleast::ix::Index::createIndex("test.idx");
    sqleast::ix::Index tree("test.idx");
    sqleast::RID rid;
    generateRID(rid);
    tree.insertEntry(1, rid);
    generateRID(rid);
    tree.insertEntry(2, rid);
    generateRID(rid);
    tree.insertEntry(3, rid);
    generateRID(rid);
    tree.insertEntry(4, rid);
    generateRID(rid);
    tree.insertEntry(5, rid);
    generateRID(rid);
    tree.insertEntry(6, rid);
    generateRID(rid);
    tree.insertEntry(7, rid);
    generateRID(rid);
    tree.insertEntry(8, rid);
    generateRID(rid);
    tree.insertEntry(9, rid);
    generateRID(rid);
    tree.insertEntry(10, rid);
    generateRID(rid);
    tree.insertEntry(11, rid);
    generateRID(rid);
    tree.insertEntry(12, rid);
    generateRID(rid);
    tree.insertEntry(13, rid);
    generateRID(rid);
    tree.insertEntry(14, rid);
    generateRID(rid);
    tree.insertEntry(15, rid);
    generateRID(rid);
    tree.insertEntry(16, rid);
    generateRID(rid);
    tree.insertEntry(17, rid);
    generateRID(rid);
    tree.insertEntry(18, rid);
    generateRID(rid);
    tree.insertEntry(19, rid);
    generateRID(rid);
    tree.insertEntry(20, rid);

    //tree.removeEntry(5);
    tree.printIndex();
    return 0;
}