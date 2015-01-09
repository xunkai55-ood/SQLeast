#include "ql/query.h"
#include "sm/systemmanager.h"
#include "sm/exception.h"

namespace sqleast {

    namespace ql {

        sm::DBManager *dbManager = nullptr;

        void SingleStringQuery::execute() {
            if (type == Q_CREATE_DB) {
                try {
                    sm::SystemManager::createDB(name);
                } catch (sm::SMException e) {
                    std::cerr << "[ERROR] DB already exists" << std::endl;
                }
            } else if (type == Q_DROP_DB) {
                try {
                    sm::SystemManager::destroyDB(name);
                } catch (sm::SMException e) {
                    std::cerr << "[ERROR] no such DB" << std::endl;
                }
            } else if (type == Q_USE_DB) {
                try {
                    sm::SystemManager::useDB(name);
                } catch (sm::SMException e) {
                    std::cerr << "[ERROR] no such DB" << std::endl;
                }
                if (dbManager != nullptr) delete dbManager;
                dbManager = new sm::DBManager(name);
            } else if (type == Q_SHOW_TABLES) {
                if (dbManager == nullptr) {
                    std::cerr << "[ERROR] DB not found" << std::endl;
                    return;
                }
                dbManager->showTables();
            } else if (type == Q_DESC_TABLE) {
                if (dbManager == nullptr) {
                    std::cerr << "[ERROR] DB not found" << std::endl;
                    return;
                }
                dbManager->descTable(name);
            } else if (type == Q_DROP_TABLE) {
                if (dbManager == nullptr) {
                    std::cerr << "[ERROR] DB not found" << std::endl;
                    return;
                }
                dbManager->dropTable(name);
            }
        }

        void CreateTableQuery::execute() {
            if (dbManager == nullptr) {
                std::cerr << "[ERROR] DB not found" << std::endl;
                return;
            }
            if (dbManager->findTable(name)) {
                std::cerr << "[ERROR] Relation exists" << std::endl;
            }
            dbManager->createTable(name, attrNum, attrs);
        }

        void InsertQuery::execute() {
            if (dbManager == nullptr) {
                std::cerr << "[ERROR] DB not found" << std::endl;
                return;
            }
            if (!dbManager->findTable(relName)) {
                std::cerr << "[ERROR] Realtion not found" << std::endl;
                return;
            }

        }
    }
}
