#include "ql/query.h"
#include "sm/systemmanager.h"
#include "sm/dbmanager.h"

namespace sqleast {

    namespace ql {

        sm::DBManager *dbManager = nullptr;

        void SingleStringQuery::execute() {
            if (type == Q_CREATE_DB) {
                sm::SystemManager::createDB(name);
            } else if (type == Q_DROP_DB) {
                sm::SystemManager::destroyDB(name);
            } else if (type == Q_USE_DB) {
                sm::SystemManager::useDB(name);
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

    }
}
