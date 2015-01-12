#ifndef SM_SM_H
#define SM_SM_H

#include "sqleast.h"
#include "sm/catalog.h"
#include <sys/stat.h>
#include <sstream>

#define REL_CATALOG "rel.catalog"
#define ATTR_CATALOG "attr.catalog"

namespace sqleast {
    namespace sm {

        class SystemManager {

        public:

            inline static std::string appendRelCatalogExt(const char *c) {
                return std::string(c) + ".rel.catalog";
            }

            inline static std::string appendAttrCatalogExt(const char *c) {
                return std::string(c) + ".attr.catalog";
            }

            inline static std::string appendDBExt(const char *c) {
                return std::string(c) + ".db";
            }

            inline static std::string appendIndexExt(const char *c, int no) {
                std::stringstream s;
                s << c << "_" << no << ".index";
                std::string res;
                s >> res;
                return res;
            }

            static void createDB(char const *dbName);
            static void useDB(char const *dbName);
            static void closeDB();
            static void destroyDB(char const *dbName);

        private:
            static int working_;
            static int isDirectory(char *path) {
                struct stat s;
                if( stat(path, &s) == 0 )
                {
                    if( s.st_mode & S_IFDIR )
                    {
                        return 1;
                    }
                }
                return 0;
            };

        };
    }
}

#endif