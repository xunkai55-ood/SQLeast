#include "rm/filehandle.h"
#include "rm/filescan.h"
#include "rm/exception.h"
#include "sm/dbmanager.h"
#include "sm/systemmanager.h"

#include <string>
#include <vector>
#include <Python/Python.h>

namespace sqleast {
    namespace sm {

        DBManager::DBManager(const char *dbName):
//                relCatalog_(rm::RecordManager::openFile(SystemManager::appendRelCatalogExt(dbName).c_str())),
//                attrCatalog_(rm::RecordManager::openFile(SystemManager::appendAttrCatalogExt(dbName).c_str()))
                  relCatalog_(rm::RecordManager::openFile(REL_CATALOG)),
                  attrCatalog_(rm::RecordManager::openFile(ATTR_CATALOG))
        {
        }

        DBManager::~DBManager() {
        }

        void DBManager::createTable(const char *relName, int attrNum, AttrInfo *attrs) {
            DataAttrInfo dataAttrInfo;
            memset(&dataAttrInfo, 0, sizeof(dataAttrInfo));
            int offset = 0;
            Record r2(FLAG_SIZE + sizeof(DataAttrInfo));
            for (int i = 0; i < attrNum; i++, attrs++) {
                strncpy(dataAttrInfo.relName, relName, MAX_NAME_LENGTH);
                strncpy(dataAttrInfo.attrName, (*attrs).attrName, MAX_NAME_LENGTH);
                dataAttrInfo.attrLength = (*attrs).attrLength;
                dataAttrInfo.attrType = (*attrs).attrType;
                dataAttrInfo.nullable = (*attrs).nullable;
                dataAttrInfo.isPrimary = (*attrs).isPrimary;
                dataAttrInfo.indexNo = i;
                dataAttrInfo.offset = offset;
                dataAttrInfo.nullBitOffset = i / 8;
                dataAttrInfo.nullBitMask = 1 << (i % 8);

                memcpy(r2.getData(), &dataAttrInfo, sizeof(DataAttrInfo));
                attrCatalog_.insertRec(r2);

                offset += (*attrs).attrLength;
            }
            RelInfo relInfo;
            memset(&relInfo, 0, sizeof(relInfo));
            strncpy(relInfo.relName, relName, MAX_NAME_LENGTH);
            relInfo.tupleLength = offset;
            relInfo.bitmapSize = attrNum / 8 + (attrNum % 8 > 0);
            relInfo.attrCount = attrNum;
            relInfo.indexCount = 0;
            Record r(FLAG_SIZE + sizeof(RelInfo));
            memcpy(r.getData(), &relInfo, sizeof(RelInfo));
            relCatalog_.insertRec(r);

            rm::RecordManager::createFile(
                    SystemManager::appendDBExt(relInfo.relName).c_str(),
                    (int)FLAG_SIZE + relInfo.tupleLength + relInfo.bitmapSize,
                    true);
        }

        void DBManager::dropTable(const char *relName) {
            /* FileHandle &handle,
                    AttrType attrType, int attrLength, int attrOffset,
                    int nullBitOffset, int nullBitMask,
                    CompOp compOp, void *value) */
            std::vector< RID > rids;
            char name[MAX_NAME_LENGTH];
            strcpy(name, relName);
            rm::FileScan attrScan(attrCatalog_, STRING, MAX_NAME_LENGTH, 0, 0, 0, EQ_OP, name);
            while (true) {
                Record &r = attrScan.next();
                if (r.rid.pageNum <= 0)
                    break;
                rids.push_back(r.rid);
            }
            for (int i = 0; i < rids.size(); i++) {
                attrCatalog_.deleteRec(rids[i]);
            }
            rids.clear();
            rm::FileScan relScan(relCatalog_, STRING, MAX_NAME_LENGTH, 0, 0, 0, EQ_OP, name);
            while (true) {
                Record &r = relScan.next();
                if (r.rid.pageNum <= 0)
                    break;
                rids.push_back(r.rid);
            }
            for (int i = 0; i < rids.size(); i++) {
                relCatalog_.deleteRec(rids[i]);
            }
            rids.clear();
        }

        void DBManager::showTables() {
            char name[MAX_NAME_LENGTH];
            rm::FileScan relScan(relCatalog_, STRING, MAX_NAME_LENGTH, 0, 0, 0, NO_OP, name);
            while (true) {
                Record &r = relScan.next();
                if (r.rid.pageNum <= 0) break;
                getCol(r.getData(), 0, MAX_NAME_LENGTH, STRING, name);
                std::cout << name << std::endl;
            }
        }

        int DBManager::findTable(const char *relName) {
            char name[MAX_NAME_LENGTH];
            strcpy(name, relName);

            rm::FileScan relScan(relCatalog_, STRING, MAX_NAME_LENGTH, 0, 0, 0, EQ_OP, name);
            while (true) {
                Record &r = relScan.next();
                if (r.rid.pageNum <= 0) break;
                return 1;
            }
            return 0;
        }

        void DBManager::descTable(const char *relName) {
            char name[MAX_NAME_LENGTH];
            strcpy(name, relName);
            rm::FileScan attrScan(attrCatalog_, STRING, MAX_NAME_LENGTH, 0, 0, 0, EQ_OP, name);
            while (true) {
                Record &r = attrScan.next();
                if (r.rid.pageNum <= 0) break;
                DataAttrInfo *dai = (DataAttrInfo*) r.getData();
                std::cout << dai->attrName << " ";
                if (dai->attrType == INT) {
                    std::cout << "int ";
                } else {
                    std::cout << "vchar(" << dai->attrLength << ") ";
                }
                if (dai->nullable == 0) {
                    std::cout << "NOT NULL ";
                }
                if (dai->isPrimary) {
                    std::cout << "primary key ";
                }
                std::cout << std::endl;
            }
        }

        void DBManager::getCol(char *data, int offset, int size, AttrType type, void *target) {
            data += offset;
            if (type == INT) {
                *(int*)target = *(int*)data;
            } else {
                strcpy((char*)target, data);
            }
        }

    }
}