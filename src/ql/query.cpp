#include "ql/query.h"
#include "sm/systemmanager.h"
#include "sm/dbmanager.h"

namespace sqleast {

    namespace ql {

        void SingleStringQuery::execute() {
            if (type == Q_CREATE_DB) {
                sm::SystemManager::createDB(name);
            } else if (type == Q_DROP_DB) {
                sm::SystemManager::destroyDB(name);
            } else if (type == Q_USE_DB) {
                sm::SystemManager::useDB(name);
            } else if (type == Q_SHOW_TABLES) {
                //dbManager.showTables();
            } else if (type == Q_DESC_TABLE) {
                //dbManager.descTable(name);
            } else if (type == Q_DROP_TABLE) {
                //dbManager.dropTable(name);
            }
        }

    }
}
