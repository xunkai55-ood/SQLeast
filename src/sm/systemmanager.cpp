#include "rm/recordmanager.h"
#include "sm/systemmanager.h"
#include "sm/exception.h"
#include "unistd.h"

#include "boost/filesystem.hpp"

namespace sqleast {
    namespace sm {

        int SystemManager::working_ = 0;

        void SystemManager::createDB(char const *dbName) {
            char name[MAX_PATH_LENGTH + 1];
            memset(name, 0, sizeof(name));
            getcwd(name, MAX_PATH_LENGTH); // system dependent
            strcat(name, PATH_SEP);
            strcat(name, dbName);
            std::cout << "[TARGET-DB]" << name << std::endl;
            if (isDirectory(name)) {
                throw DBExistsException();
            }
            mkdir(name, 0777);
            chdir(dbName);
//            rm::RecordManager::createFile(appendRelCatalogExt(dbName).c_str(), sizeof(RelInfo) + FLAG_SIZE);
//            rm::RecordManager::createFile(appendAttrCatalogExt(dbName).c_str(), sizeof(DataAttrInfo) + FLAG_SIZE);
            rm::RecordManager::createFile(REL_CATALOG, sizeof(RelInfo) + FLAG_SIZE);
            rm::RecordManager::createFile(ATTR_CATALOG, sizeof(DataAttrInfo) + FLAG_SIZE);
            chdir("..");
        }

        void SystemManager::useDB(char const *dbName) {
            if (working_)
                closeDB();
            char name[MAX_PATH_LENGTH + 1];
            memset(name, 0, sizeof(name));
            getcwd(name, MAX_PATH_LENGTH); // system dependent
            strcat(name, PATH_SEP);
            strcat(name, dbName);
            std::cout << "[TARGET-DB]" << name << std::endl;
            if (!isDirectory(name)) {
                throw DBNotExistsException();
            }
            chdir(dbName);
            working_ = 1;
        }

        void SystemManager::closeDB() {
            if (!working_) return;
            chdir("..");
        }

        void SystemManager::destroyDB(char const *dbName) {
            if (working_)
                closeDB();
            char name[MAX_PATH_LENGTH + 1];
            memset(name, 0, sizeof(name));
            getcwd(name, MAX_PATH_LENGTH); // system dependent
            strcat(name, PATH_SEP);
            strcat(name, dbName);
            std::cout << "[TARGET-DB]" << name << std::endl;
            if (!isDirectory(name)) {
                throw DBNotExistsException();
            }
            boost::filesystem::remove_all(boost::filesystem::path(dbName));
        }
    }
}