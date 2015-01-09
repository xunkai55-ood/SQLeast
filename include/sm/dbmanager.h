#ifndef SM_DBMANAGER_H
#define SM_DBMANAGER_H

#include "catalog.h"
#include "rm/filehandle.h"
#include "rm/recordmanager.h"

namespace sqleast {
    namespace sm {

        class DBManager {

        public:

            DBManager(const char *dbName);
            ~DBManager();
            void createTable(const char *relName, int attrNum, AttrInfo *attrs);
            void dropTable(const char *relName);
            void createIndex(const char *relName, const char *attrName);
            void dropIndex(const char *relName, const char *attrName);

            void showTables();
            void descTable(const char *relName);

            int findTable(const char *relName);

            void getCol(char *data, int offset, int size, AttrType type, void *target);

        private:
            rm::FileHandle relCatalog_, attrCatalog_;
        };
    }
}


#endif