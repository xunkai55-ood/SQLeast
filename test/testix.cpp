#include <iostream>
#include "ix/index.h"
#include "ix/indexscan.h"

using namespace std;

int main()
{
    sqleast::ix::Index::createIndex("test.idx");
    sqleast::ix::Index Tree("test.idx");
    Tree.printIndex();
    return 0;
}