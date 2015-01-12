#ifndef SM_DBMANAGER_H
#define SM_DBMANAGER_H

#include "catalog.h"
#include "rm/filehandle.h"
#include "rm/recordmanager.h"
#include "sm/systemmanager.h"

namespace sqleast {
    namespace sm {

        class DBHandle {

        public:

            DBHandle(const char *dbName);
            ~DBHandle();
            void createTable(const char *relName, int attrNum, const char *primary, AttrInfo *attrs);
            void dropTable(const char *relName);
            void createIndex(const char *relName, const char *attrName);
            void dropIndex(const char *relName, const char *attrName);

            void showTables();
            void descTable(const char *relName);

            int findTable(const char *relName);

            int loadRelAttrInfo(const char *relName, DataAttrInfo *attrInfoArr);
            RelInfo getRelInfo(const char *relName);

            void loadCol(char *data, int offset, int size, AttrType type, void *target);

            RID findKey(char *relName, DataAttrInfo *dai, Value *value);

            inline rm::FileHandle openRelFile(char *relName) {
                return rm::RecordManager::openFile(SystemManager::appendDBExt(relName).c_str());
            }

//            inline rm::FileHandle openIndexFile(char *relName) {
//                return rm::RecordManager::openFile(SystemManager::appendDBExt(name).c_str());
//            }

        private:
            rm::FileHandle relCatalog_, attrCatalog_;
        };
    }
}


#endif