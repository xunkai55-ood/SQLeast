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
            Q_DESC_TABLE,

            Q_INSERT
        };

        struct StructuredQuery {
            SupportedQueryType type;
        };

        struct SingleStringQuery: public StructuredQuery {
            char name[MAX_NAME_LEN + 1];
            void execute();
        };

        struct CreateTableQuery: public StructuredQuery {
            char name[MAX_NAME_LENGTH + 1];
            AttrInfo attrs[MAX_ATTR_NUM];
            int attrNum;
            void execute();
        };

        struct InsertQuery: public StructuredQuery {
            char relName[MAX_NAME_LENGTH + 1];
            void *attrs[MAX_ATTR_NUM];
            AttrType *types[MAX_ATTR_NUM];
            int attrLengths[MAX_ATTR_NUM];
            int attrNum;
            void execute();
        };

    }
}

#endif