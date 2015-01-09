#ifndef QL_QUERY_H
#define QL_QUERY_H

#include <vector>
#include <sqleast.h>
#include <vector>

#include "sm/dbmanager.h"

namespace sqleast {

//    extern sm::DBManager dbManager;

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
            virtual void execute() {std::cerr << "NOT IMPL" << std::endl;}
        };

        struct SingleStringQuery: public StructuredQuery {
            char name[MAX_NAME_LENGTH + 1];
            void execute();
        };

        struct CreateTableQuery: public StructuredQuery {
            char name[MAX_NAME_LENGTH + 1];
            AttrInfo attrs[MAX_ATTR_NUM];
            int attrNum;

            void execute();
        };

        /* insert */
        struct InsertAttrItem {
            int iValue;
            std::string sValue;
            AttrType type;
        };

        typedef std::vector< InsertAttrItem > InsertItem;

        struct InsertQuery: public StructuredQuery {
            char relName[MAX_NAME_LENGTH + 1];
            std::vector<InsertItem> v;

//            void execute();
        };

        /* where item */
        struct WhereItem {
            CompOp op;
            int iValue;
            std::string sValue;
            AttrType type;
            std::string op1;
            std::string op2;
            int useOp2; // 1 means op1 CompOp op2
        };

        enum LogicOp {AND_OP, OR_OP};

        struct WhereClause {
            std::vector<WhereItem> item;
            std::vector<LogicOp> op;
            // op.size == item.size - 1;
        };

        /* delete */
        struct DeleteQuery {
            string relName;
            WhereClause where;
        };

    }
}

#endif