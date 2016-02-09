#include "sm/systemmanager.h"
#include "sm/dbhandle.h"

int main() {
    sqleast::sm::SystemManager::destroyDB("testing");
    std::cout << sizeof(sqleast::sm::RelInfo) << std::endl;
    std::cout << sizeof(sqleast::sm::DataAttrInfo) << std::endl;
    sqleast::sm::SystemManager::createDB("testing");
    sqleast::sm::SystemManager::useDB("testing");

    sqleast::AttrInfo attrs[3];

    attrs[0].attrName = new char[sqleast::MAX_NAME_LENGTH];
    memcpy(attrs[0].attrName, "id", sqleast::MAX_NAME_LENGTH);
    attrs[0].attrLength = sizeof(int);
    attrs[0].attrType = sqleast::INT;
    attrs[0].nullable = 0;

    attrs[1].attrName = new char[sqleast::MAX_NAME_LENGTH];
    memcpy(attrs[1].attrName, "name", sqleast::MAX_NAME_LENGTH);
    attrs[1].attrLength = 100;
    attrs[1].attrType = sqleast::STRING;
    attrs[1].nullable = 0;

    attrs[2].attrName = new char[sqleast::MAX_NAME_LENGTH];
    memcpy(attrs[2].attrName, "nation", sqleast::MAX_NAME_LENGTH);
    attrs[2].attrLength = 3;
    attrs[2].attrType = sqleast::STRING;
    attrs[2].nullable = 1;

    sqleast::sm::DBManager manager("testing");
    manager.createTable("publisher", 3, attrs);
    manager.createTable("crap", 2, attrs + 1);
    manager.dropTable("publisher");

    sqleast::sm::SystemManager::closeDB();
    return 0;
}
