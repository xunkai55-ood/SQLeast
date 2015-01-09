#ifndef QL_QUERY_H
#define QL_QUERY_H

#include <sqleast.h>

namespace sqleast {

    extern sm::DBManager dbManager;

    namespace ql {
        enum SupportedQueryType {
            Q_CREATE_DB,
            Q_DROP_DB,
            Q_USE_DB,
            Q_SHOW_TABLES,
            Q_CREATE_TABLE,
            Q_DROP_TABLE,
            Q_DESC_TABLE
        };

        struct StructuredQuery {
            SupportedQueryType type;
            virtual void execute();
        };

        struct SingleStringQuery: public StructuredQuery {
            char name[MAX_NAME_LEN + 1];
            void execute();
        };

        struct CreateTableQuery: public StructuredQuery {
            char dbName[MAX_NAME_LEN + 1];
            AttrInfo attrs[MAX_ATTR_NUM];
            void execute();
        };

    }
}

#endif