#include <iostream>
#include "sqleast.h"
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
    using namespace sqleast;
    return 0;
}
