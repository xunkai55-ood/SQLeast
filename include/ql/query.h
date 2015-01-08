#ifndef QL_QUERY_H
#define QL_QUERY_H

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

        struct SingleDBQuery: public StructuredQuery {
            char dbName[MAX_NAME_LEN + 1];
        };

    }
}

#endif