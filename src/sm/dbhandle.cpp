#include "rm/filehandle.h"
#include "rm/filescan.h"
#include "rm/exception.h"
#include "sm/dbhandle.h"
#include "sm/systemmanager.h"
#include "ql/printer.h"

extern Printer printer;
extern Printer err;

#include <vector>
#include <ix/index.h>

using namespace std;

namespace sqleast {
    namespace sm {

        DBHandle::DBHandle(const char *dbName):
//                relCatalog_(rm::RecordManager::openFile(SystemManager::appendRelCatalogExt(dbName).c_str())),
//                attrCatalog_(rm::RecordManager::openFile(SystemManager::appendAttrCatalogExt(dbName).c_str()))
                  relCatalog_(rm::RecordManager::openFile(REL_CATALOG)),
                  attrCatalog_(rm::RecordManager::openFile(ATTR_CATALOG))
        {
        }

        DBHandle::~DBHandle() {
        }

        void DBHandle::createTable(const char *relName, int attrNum, const char *primary, AttrInfo *attrs) {
            DataAttrInfo dataAttrInfo;
            memset(&dataAttrInfo, 0, sizeof(dataAttrInfo));
            int offset = 0;
            Record r2(FLAG_SIZE + sizeof(DataAttrInfo));
            AttrInfo *a = attrs;
            int tupleLength = 0;
            int primaryFound = 0;
            for (int i = 0; i < attrNum; i++, a++) {
                if ((*a).attrType == STRING)
                    tupleLength += (*a).attrLength;
                else if ((*a).attrType == INT)
                    tupleLength += sizeof(int);
                else
                    tupleLength += sizeof(double);
                if (primary != nullptr && !strcmp(primary, (*a).attrName) && (*a).attrType == INT) {
                    primaryFound = 1;
                }
            }
            if (primary != nullptr && !primaryFound) {
                err << "[ERROR] primary key invaild" << endl;
            }

            for (int i = 0; i < attrNum; i++, attrs++) {
                strncpy(dataAttrInfo.relName, relName, MAX_NAME_LENGTH);
                strncpy(dataAttrInfo.attrName, (*attrs).attrName, MAX_NAME_LENGTH);
                if ((*attrs).attrType == STRING)
                    dataAttrInfo.attrLength = (*attrs).attrLength;
                else if ((*attrs).attrType == INT)
                    dataAttrInfo.attrLength = sizeof(int);
                else
                    dataAttrInfo.attrLength = sizeof(double);
                dataAttrInfo.attrType = (*attrs).attrType;
                dataAttrInfo.nullable = (*attrs).nullable;
                dataAttrInfo.nullBitOffset = (i >> 3) + tupleLength;
                dataAttrInfo.nullBitMask = (1 << (i & 7));
                if (primary != nullptr) {
                    dataAttrInfo.isPrimary = (strcmp((*attrs).attrName, primary) == 0);
                    dataAttrInfo.indexNo = 0;
                    ix::Index::createIndex(SystemManager::appendIndexExt(relName, 0).c_str());
                } else {
                    dataAttrInfo.isPrimary = false;
                    dataAttrInfo.indexNo = -1;
                }
                dataAttrInfo.offset = offset;

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
            relInfo.indexCount = primaryFound;
            if (primary != nullptr)
                strncpy(relInfo.primaryKey, primary, MAX_NAME_LENGTH);
            else
                strncpy(relInfo.primaryKey, "", MAX_NAME_LENGTH);
            Record r(FLAG_SIZE + sizeof(RelInfo));
            memcpy(r.getData(), &relInfo, sizeof(RelInfo));
            relCatalog_.insertRec(r);

            rm::RecordManager::createFile(
                    SystemManager::appendDBExt(relInfo.relName).c_str(),
                    (int)FLAG_SIZE + relInfo.tupleLength + relInfo.bitmapSize,
                    true);
        }

        void DBHandle::dropTable(const char *relName) {
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

        void DBHandle::showTables() {
            char name[MAX_NAME_LENGTH];
            rm::FileScan relScan(relCatalog_, STRING, MAX_NAME_LENGTH, 0, 0, 0, NO_OP, name);
            while (true) {
                Record &r = relScan.next();
                if (r.rid.pageNum <= 0) break;
                loadCol(r.getData(), 0, MAX_NAME_LENGTH, STRING, name);
                printer << name << endl;
            }
        }

        int DBHandle::findTable(const char *relName) {
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

        void DBHandle::descTable(const char *relName) {
            char name[MAX_NAME_LENGTH];
            strcpy(name, relName);
            DataAttrInfo infoArr[MAX_ATTR_NUM];
            memset(infoArr, 0, sizeof(infoArr));
            int n = loadRelAttrInfo(relName, infoArr);
            printer << "Total columns: " << n << endl;
            for (int i = 0; i < n; i++) {
                DataAttrInfo *dai = &infoArr[i];
                printer << dai->attrName << " ";
                if (dai->attrType == INT) {
                    printer << "int ";
                } else if (dai->attrType == STRING) {
                    printer << "vchar(" << dai->attrLength - 1 << ") ";
                } else {
                    printer << "float" << dai->attrLength;
                }
                if (dai->nullable == 0) {
                    printer << "NOT NULL ";
                }
                if (dai->isPrimary) {
                    printer << "primary key ";
                }
                printer << endl;
            }
        }

        void DBHandle::loadCol(char *data, int offset, int size, AttrType type, void *target) {
            data += offset;
            if (type == INT) {
                *(int*)target = *(int*)data;
            } else if (type == FLOAT) {
                *(double*)target = *(double*)data;
            } else {
                strncpy((char*)target, data, (size_t) size);
            }
        }

        int DBHandle::loadRelAttrInfo(const char *relName, DataAttrInfo *attrInfoArr) {
            char name[MAX_NAME_LENGTH];
            strcpy(name, relName);
            rm::FileScan attrScan(attrCatalog_, STRING, MAX_NAME_LENGTH, 0, 0, 0, EQ_OP, name);
            int n = 0;
            while (true) {
                Record &r = attrScan.next();
                if (r.rid.pageNum <= 0) break;
                attrInfoArr[n] = *(DataAttrInfo*) r.getData();
                n++;
            }
            return n;
        }

        RelInfo DBHandle::getRelInfo(const char *relName) {
            char name[MAX_NAME_LENGTH];
            strcpy(name, relName);

            rm::FileScan relScan(relCatalog_, STRING, MAX_NAME_LENGTH, 0, 0, 0, EQ_OP, name);
            Record &r = relScan.next();
            return *(RelInfo*)(r.getData());
        }

        RID DBHandle::findKey(char *relName, DataAttrInfo *dai, Value *value) {
            if (dai->indexNo < 0) {
                rm::FileHandle db = openRelFile(relName);
                rm::FileScan dbScan(
                        db,
                        value->type,
                        dai->attrLength,
                        dai->offset,
                        0,
                        0,
                        EQ_OP,
                        value->data
                );
                Record &r = dbScan.next();
                return r.rid;
            } else {
                // TODO check index
            }
            return RID(-1, -1);
        }
    }
}