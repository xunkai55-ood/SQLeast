#ifndef QL_QUERY_H
#define QL_QUERY_H

#include <sqleast.h>

namespace sqleast {

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
        };

        struct SingleStringQuery: public StructuredQuery {
            char name[MAX_NAME_LENGTH + 1];
        };

        struct CreateTableQuery: public StructuredQuery {
            char name[MAX_NAME_LENGTH + 1];
            AttrInfo attrs[MAX_ATTR_NUM];
            int attrNum;
        };

    }
}

#endif