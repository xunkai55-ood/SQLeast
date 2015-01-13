#include <rm/filescan.h>
#include <vector>
#include <ix/index.h>
#include "sm/systemmanager.h"
#include "sm/exception.h"
#include "sm/catalog.h"
#include "sm/dbhandle.h"
#include "frontend/parser_internal.h"
#include "ql/printer.h"

extern Printer printer;
extern Printer err;

namespace sqleast {
    namespace ql {

        using namespace std;
        sm::DBHandle *dbHandle = nullptr;
        AttrInfo attrInfo[MAX_ATTR_NUM];
        sm::DataAttrInfo dataAttrInfo[MAX_ATTR_NUM];
        sm::DataAttrInfo manyDataAttrInfo[MAX_COND_NUM][MAX_ATTR_NUM];
        sm::RelInfo manyRelInfo[MAX_COND_NUM];
        char *relName, *attrName, *primary;
        int attrNum, i;
        NODE *curr, *iter;
        int selected[MAX_ATTR_NUM];
        int queryOrder[MAX_COND_NUM];
        int joinPoint[MAX_COND_NUM][2]; // jp[i][0] --> cond[qo[i - 1]].lhs join cond[qo[i]].something
        int condFlag[MAX_COND_NUM], attrCnt[MAX_COND_NUM];
        Record *joined[MAX_COND_NUM];
        int needDel[MAX_COND_NUM];
        char *relNames[MAX_COND_NUM];
        rm::FileHandle *dbFiles[MAX_COND_NUM];
        int relNum;
        int selectCnt;

        struct CondSymbol {
            int attrId;
            int relId;
            CompOp op;
            void *rhsValue;
            AttrType rhsValueType;
            int rhsRelId;
            int rhsAttrId;
        } conds[MAX_COND_NUM], joinConds[MAX_COND_NUM];
        int condsPtr = 0, joinCondsPtr = 0;

        void makeValue(NODE *n, Value &v) {
            if (n->u.VALUE.type == INT) {
                v.data = (void*)&(n->u.VALUE.ival);
            } else if (n->u.VALUE.type == STRING) {
                v.data = (void*)n->u.VALUE.sval;
            } else if (n->u.VALUE.type == FLOAT) {
                v.data = (void*)&(n->u.VALUE.rval);
            } else {
                v.data = nullptr;
            }
            v.type = n->u.VALUE.type;
        }

        void copyValue(char *p, Value &v, int maxLen) {
            if (v.type == STRING) {
                strncpy(p, (char*)v.data, maxLen);
            } else if (v.type == INT) {
                *(int*)p = *(int*)v.data;
            } else if (v.type == FLOAT) {
                *(real*)p = *(real*)v.data;
            } else {
                err << "[FATAL] copy null" << endl;
            }
        }

        void executeInsert(NODE *n) {

            int j;
            NODE *vCurr, *vIter;

            relName = n->u.INSERT.relname;
            if (!dbHandle->findTable(relName)) {
                err << "[ERROR] relation not found" << endl;
                return;
            }
            memset(dataAttrInfo, 0, sizeof(dataAttrInfo));
            int attrNum = dbHandle->loadRelAttrInfo(relName, dataAttrInfo);
            sm::RelInfo relInfo = dbHandle->getRelInfo(relName);
            ix::Index *primaryIndex = nullptr;
            if (*relInfo.primaryKey != '\0')
                primaryIndex = new ix::Index(sm::SystemManager::appendIndexExt(relName, 0).c_str());
            /* first iteration: input validation */
            for (i = 0, iter = n->u.INSERT.tuplelist; iter != nullptr; iter = iter->u.LIST.next, i++) {
                for (j = 0, vIter = iter->u.LIST.curr; vIter != nullptr; vIter = vIter->u.LIST.next, j++) {
                    curr = vIter->u.LIST.curr;
                    if (j >= attrNum) {
                        err << "[ERROR] too many columns in record " << i + 1 << ", stopping insertion" << endl;
                        return;
                    } else {
                        if (curr->u.VALUE.type == NULLV) {
                            if (!dataAttrInfo[j].nullable) {
                                err << "[ERROR] can't be null" << endl;
                            } else {
                                continue;
                            }
                        }
                        if (curr->u.VALUE.type != NULLV && curr->u.VALUE.type != dataAttrInfo[j].attrType) {
                            err << "[ERROR] type dismatch in record " << i + 1 << " column " << j + 1 << endl;
                            return;
                        }
                        if (dataAttrInfo[j].isPrimary) {
                            Value v;
                            makeValue(curr, v);
                            RID rid;
                            if (primaryIndex != nullptr)
                                rid = primaryIndex->searchEntry(*(int*)v.data);
                            else
                                rid = dbHandle->findKey(relName, dataAttrInfo + j, &v);
                            if (rid.pageNum > 0) {
                                err << "[ERROR] primary key conflict" << endl;
                                return;
                            }
                        }
                    }
                }
                if (j < attrNum) {
                    err << "[ERROR] too less columns in record " << i + 1 << ", at least " << attrNum << endl;
                    return;
                }
            }
            /* second iteration: make record and insert */
            Record r(FLAG_SIZE + relInfo.tupleLength + relInfo.bitmapSize);
            rm::FileHandle dbFile = dbHandle->openRelFile(relName);
            Value v;
            char *p;
            int cnt = 0;
            for (i = 0, iter = n->u.INSERT.tuplelist; iter != nullptr; iter = iter->u.LIST.next, i++) {
                int primaryKey = -1;
                r.clear();
                for (j = 0, vIter = iter->u.LIST.curr; vIter != nullptr; vIter = vIter->u.LIST.next, j++) {
                    curr = vIter->u.LIST.curr;
                    p = r.getData() + dataAttrInfo[j].offset;
                    makeValue(curr, v);
                    if (v.type != NULLV) {
                        copyValue(p, v, dataAttrInfo[j].attrLength);
                    } else {
                        p = r.getData() + dataAttrInfo[j].nullBitOffset;
                        *p |= dataAttrInfo[j].nullBitMask;
                    }
                    if (dataAttrInfo[j].isPrimary) primaryKey = *(int*)v.data;
                }
                dbFile.insertRec(r);
                if (primaryIndex != nullptr) primaryIndex->insertEntry(primaryKey, r.rid);
//                cout << "<INSERT> RID(" << r.rid.pageNum << ", " << r.rid.slotNum << ") No." << i + 1 << endl;
                cnt++;
            }
            // TODO insert to index
            if (primaryIndex != nullptr)
                delete primaryIndex;
            printer << "<INSERT> " << cnt << " row(s)" << endl;
        }

        void executeDelete(NODE *n) {

            int marker = -1;
            relName = n->u.DELETE.relname;
            if (!dbHandle->findTable(relName)) {
                err << "[ERROR] relation not found" << endl;
                return;
            }
            char *joinRelName = nullptr;
            rm::FileHandle dbFile = dbHandle->openRelFile(relName);
            sm::RelInfo relInfo = dbHandle->getRelInfo(relName);
            memset(dataAttrInfo, 0, sizeof(dataAttrInfo));
            int attrNum = dbHandle->loadRelAttrInfo(relName, dataAttrInfo);
            int targetCol = -1;
            CompOp op;
            Value v;
            void *vValue = nullptr;
            condsPtr = 0;
            ix::Index *primaryIndex = nullptr;
            if (*relInfo.primaryKey != '\0')
                primaryIndex = new ix::Index(sm::SystemManager::appendIndexExt(relName, 0).c_str());

            /* construct conds table */
            for (i = 0, iter = n->u.DELETE.conditionlist; iter != nullptr; iter = iter->u.LIST.next, i++) {
                curr = iter->u.LIST.curr;
                conds[condsPtr].op = curr->u.CONDITION.op;
                conds[condsPtr].rhsRelId = 0;
                conds[condsPtr].rhsAttrId = -1;
                conds[condsPtr].attrId = -1;

                char *attrName = curr->u.CONDITION.lhsRelattr->u.RELATTR.attrname;
                for (int j = 0; j < attrNum; j++)
                    if (!strcmp(dataAttrInfo[j].attrName, attrName)) {
                        conds[condsPtr].attrId = j;
                    }
                if (curr->u.CONDITION.rhsRelattr != nullptr) {
                    char *rhsAttrName = curr->u.CONDITION.rhsRelattr->u.RELATTR.attrname;
                    for (int j = 0; j < attrNum; j++) {
                        if (!strcmp(dataAttrInfo[j].attrName, rhsAttrName)) {
                            conds[condsPtr].rhsAttrId = j;
                        }
                    }
                }
                if (curr->u.CONDITION.rhsValue != nullptr) {
                    makeValue(curr->u.CONDITION.rhsValue, v);
                    conds[condsPtr].rhsValue = v.data;
                    conds[condsPtr].rhsValueType = v.type;
                } else {
                    conds[condsPtr].rhsValue = nullptr;
                    conds[condsPtr].rhsValueType = INT;
                }

                if ((curr->u.CONDITION.rhsRelattr != nullptr && conds[condsPtr].rhsAttrId < 0) || conds[condsPtr].attrId < 0) {
                    err << "[ERROR] unknown column" << endl;
                    return;
                }
                condsPtr++;
            }

            /* find first condition */
            for (int i = 0; i < condsPtr; i++) {
                if (conds[i].rhsAttrId == -1) {
                    marker = i;
                    break;
                }
            }

            if (marker < 0) {
                op = NO_OP;
                targetCol = 0;
            } else {
                targetCol = conds[marker].attrId;
                op = conds[marker].op;
            }

            rm::FileScan dbScan(
                    dbFile,
                    dataAttrInfo[targetCol].attrType,
                    dataAttrInfo[targetCol].attrLength,
                    dataAttrInfo[targetCol].offset,
                    dataAttrInfo[targetCol].nullBitOffset,
                    dataAttrInfo[targetCol].nullBitMask,
                    op,
                    conds[marker].rhsValue
            );

            int cnt = 0;

            int primaryKeyOffset = -1;
            for (int j = 0; j < attrNum; j++) {
                if (dataAttrInfo[j].isPrimary) {
                    primaryKeyOffset = dataAttrInfo[j].offset;
                }
            }

            while (true) {
                Record &r = dbScan.next();
                if (r.rid.pageNum <= 0) break;
                /* check other contraints */
                int pass = 1;
                for (int i = 0; i < condsPtr; i++) {
                    if (i == marker) continue;
                    op = conds[i].op;
                    sm::DataAttrInfo &lhsInfo = dataAttrInfo[conds[i].attrId];
                    if (op == NO_OP) {
                        continue;
                    } else if (op == IS_NULL_OP) {
                        if (lhsInfo.nullable && ((*(r.getData() + lhsInfo.nullBitOffset)) & lhsInfo.nullBitMask))
                            continue;
                    } else if (op == NOT_NULL_OP) {
                        if (!lhsInfo.nullable || !((*(r.getData() + lhsInfo.nullBitOffset)) & lhsInfo.nullBitMask))
                            continue;
                    } else if (conds[i].rhsAttrId >= 0) { // comps between to col
                        sm::DataAttrInfo &rhsInfo = dataAttrInfo[conds[i].rhsAttrId];
                        if (lhsInfo.attrType != rhsInfo.attrType) {
                            err << "[ERROR] uncomparable attributes" << endl;
                            return;
                        }
                        int flag;
                        if (lhsInfo.attrType == INT) {
                            int llv = *(int *)(r.getData() + lhsInfo.offset);
                            int rrv = *(int *)(r.getData() + rhsInfo.offset);
                            flag = llv < rrv ? -1 : llv > rrv ? 1 : 0;
                        } else {
                            char *llv = r.getData() + lhsInfo.offset;
                            char *rrv = r.getData() + rhsInfo.offset;
                            flag = strncmp(llv, rrv, max(lhsInfo.attrLength, rhsInfo.attrLength));
                        }
                        if (flag == 0) {
                            if (op == EQ_OP || op == LE_OP || op == GE_OP)
                                continue;
                        } else if (flag < 0) {
                            if (op == LT_OP || op == LE_OP || op == NE_OP)
                                continue;
                        } else {
                            if (op == GT_OP || op == GE_OP || op == NE_OP)
                                continue;
                        }

                    } else if (conds[i].rhsValue != nullptr) {
                        if (lhsInfo.attrType != conds[i].rhsValueType) {
                            err << "[ERROR] uncomparable attributes" << endl;
                            return;
                        }
                        int flag;
                        if (lhsInfo.attrType == INT) {
                            int llv = *(int *)(r.getData() + lhsInfo.offset);
                            int rrv = *(int *)(conds[i].rhsValue);
                            flag = llv < rrv ? -1 : llv > rrv ? 1 : 0;
                        } else {
                            char *llv = r.getData() + lhsInfo.offset;
                            char *rrv = (char*)(conds[i].rhsValue);
                            flag = strncmp(llv, rrv, lhsInfo.attrLength);
                        }
                        if (flag == 0) {
                            if (op == EQ_OP || op == LE_OP || op == GE_OP)
                                continue;
                        } else if (flag < 0) {
                            if (op == LT_OP || op == LE_OP || op == NE_OP)
                                continue;
                        } else {
                            if (op == GT_OP || op == GE_OP || op == NE_OP)
                                continue;
                        }
                    } else {
                        err << "[ERROR] strange, no valid situation" << endl;
                    }
                    pass = 0;
                    break;
                }
                if (pass) {
                    cnt ++;
//                    cout << "<DELETE> RID(" << r.rid.pageNum << ", " << r.rid.slotNum << ")" << endl;
                    if (primaryIndex != nullptr) {
                        int pk = *(int*)(r.getData() + primaryKeyOffset);
//                        primaryIndex->removeEntry(pk);
                        // TODO fix this bug;
                    }
                    dbFile.deleteRec(r);
                }
            }
            delete primaryIndex;
            printer << "<DELETE> " << cnt << " row(s)" << endl;
        }

        void executeUpdate(NODE *n) {

            int marker = -1;
            relName = n->u.UPDATE.relname;
            if (!dbHandle->findTable(relName)) {
                err << "[ERROR] relation not found" << endl;
                return;
            }
            char *joinRelName = nullptr;
            rm::FileHandle dbFile = dbHandle->openRelFile(relName);
            sm::RelInfo relInfo = dbHandle->getRelInfo(relName);
            memset(dataAttrInfo, 0, sizeof(dataAttrInfo));
            int attrNum = dbHandle->loadRelAttrInfo(relName, dataAttrInfo);
            int targetCol = -1;
            int writeBackCol = -1;
            int rhsCol = -1;
            CompOp op;
            Value v, writeBackValue;
            void *vValue = nullptr;
            condsPtr = 0;

            for (int j = 0; j < attrNum; j++)
                if (!strcmp(dataAttrInfo[j].attrName, n->u.UPDATE.relattr->u.RELATTR.attrname)) {
                    writeBackCol = j;
                    break;
                }
            if (writeBackCol < 0) {
                err << "[ERROR] cannot set to an unexisted column" << endl;
                return;
            }
            if (n->u.UPDATE.relorvalue->u.RELATTR_OR_VALUE.relattr == nullptr) { // set a value
                NODE *vNode = n->u.UPDATE.relorvalue->u.RELATTR_OR_VALUE.value;
                if (vNode->u.VALUE.type != dataAttrInfo[writeBackCol].attrType) {
                    err << "[ERROR] type dismatches and cannot set" << endl;
                    return;
                }
                makeValue(vNode, writeBackValue);
            } else { // set a col
                for (int j = 0; j < attrNum; j++)
                    if (!strcmp(dataAttrInfo[j].attrName, n->u.UPDATE.relorvalue->u.RELATTR_OR_VALUE.relattr->u.RELATTR.attrname)) {
                        rhsCol = j;
                        break;
                    }
                if (dataAttrInfo[writeBackCol].attrType != dataAttrInfo[rhsCol].attrType) {
                    err << "[ERROR] type dismatches and cannot set" << endl;
                    return;
                }
            }

            /* construct conds table */
            for (i = 0, iter = n->u.UPDATE.conditionlist; iter != nullptr; iter = iter->u.LIST.next, i++) {
                curr = iter->u.LIST.curr;
                conds[condsPtr].op = curr->u.CONDITION.op;
                conds[condsPtr].rhsRelId = 0;
                conds[condsPtr].rhsAttrId = -1;
                conds[condsPtr].attrId = -1;

                char *attrName = curr->u.CONDITION.lhsRelattr->u.RELATTR.attrname;
                for (int j = 0; j < attrNum; j++)
                    if (!strcmp(dataAttrInfo[j].attrName, attrName)) {
                        conds[condsPtr].attrId = j;
                    }
                if (curr->u.CONDITION.rhsRelattr != nullptr) {
                    char *rhsAttrName = curr->u.CONDITION.rhsRelattr->u.RELATTR.attrname;
                    for (int j = 0; j < attrNum; j++) {
                        if (!strcmp(dataAttrInfo[j].attrName, rhsAttrName)) {
                            conds[condsPtr].rhsAttrId = j;
                        }
                    }
                }
                if (curr->u.CONDITION.rhsValue != nullptr) {
                    makeValue(curr->u.CONDITION.rhsValue, v);
                    conds[condsPtr].rhsValue = v.data;
                    conds[condsPtr].rhsValueType = v.type;
                } else {
                    conds[condsPtr].rhsValue = nullptr;
                    conds[condsPtr].rhsValueType = INT;
                }

                if ((curr->u.CONDITION.rhsRelattr != nullptr && conds[condsPtr].rhsAttrId < 0) || conds[condsPtr].attrId < 0) {
                    err << "[ERROR] unknown column" << endl;
                    return;
                }
                condsPtr++;
            }

            /* find first condition */
            for (int i = 0; i < condsPtr; i++) {
                if (conds[i].rhsAttrId == -1) {
                    marker = i;
                    break;
                }
            }

            if (marker < 0) {
                op = NO_OP;
                targetCol = 0;
            } else {
                targetCol = conds[marker].attrId;
                op = conds[marker].op;
            }

            rm::FileScan dbScan(
                    dbFile,
                    dataAttrInfo[targetCol].attrType,
                    dataAttrInfo[targetCol].attrLength,
                    dataAttrInfo[targetCol].offset,
                    dataAttrInfo[targetCol].nullBitOffset,
                    dataAttrInfo[targetCol].nullBitMask,
                    op,
                    conds[marker].rhsValue
            );
            int cnt =0;

            while (true) {
                Record &r = dbScan.next();
                if (r.rid.pageNum <= 0) break;
                /* check other contraints */
                int pass = 1;
                for (int i = 0; i < condsPtr; i++) {
                    if (i == marker) continue;
                    op = conds[i].op;
                    sm::DataAttrInfo &lhsInfo = dataAttrInfo[conds[i].attrId];
                    if (op == NO_OP) {
                        continue;
                    } else if (op == IS_NULL_OP) {
                        if (lhsInfo.nullable && ((*(r.getData() + lhsInfo.nullBitOffset)) & lhsInfo.nullBitMask))
                            continue;
                    } else if (op == NOT_NULL_OP) {
                        if (!lhsInfo.nullable || !((*(r.getData() + lhsInfo.nullBitOffset)) & lhsInfo.nullBitMask))
                            continue;
                    } else if (conds[i].rhsAttrId >= 0) { // comps between to col
                        sm::DataAttrInfo &rhsInfo = dataAttrInfo[conds[i].rhsAttrId];
                        if (lhsInfo.attrType != rhsInfo.attrType) {
                            err << "[ERROR] uncomparable attributes" << endl;
                            return;
                        }
                        int flag;
                        if (lhsInfo.attrType == INT) {
                            int llv = *(int *)(r.getData() + lhsInfo.offset);
                            int rrv = *(int *)(r.getData() + rhsInfo.offset);
                            flag = llv < rrv ? -1 : llv > rrv ? 1 : 0;
                        } else {
                            char *llv = r.getData() + lhsInfo.offset;
                            char *rrv = r.getData() + rhsInfo.offset;
                            flag = strncmp(llv, rrv, max(lhsInfo.attrLength, rhsInfo.attrLength));
                        }
                        if (flag == 0) {
                            if (op == EQ_OP || op == LE_OP || op == GE_OP)
                                continue;
                        } else if (flag < 0) {
                            if (op == LT_OP || op == LE_OP || op == NE_OP)
                                continue;
                        } else {
                            if (op == GT_OP || op == GE_OP || op == NE_OP)
                                continue;
                        }

                    } else if (conds[i].rhsValue != nullptr) {
                        if (lhsInfo.attrType != conds[i].rhsValueType) {
                            err << "[ERROR] uncomparable attributes" << endl;
                            return;
                        }
                        int flag;
                        if (lhsInfo.attrType == INT) {
                            int llv = *(int *)(r.getData() + lhsInfo.offset);
                            int rrv = *(int *)(conds[i].rhsValue);
                            flag = llv < rrv ? -1 : llv > rrv ? 1 : 0;
                        } else {
                            char *llv = r.getData() + lhsInfo.offset;
                            char *rrv = (char*)(conds[i].rhsValue);
                            flag = strncmp(llv, rrv, lhsInfo.attrLength);
                        }
                        if (flag == 0) {
                            if (op == EQ_OP || op == LE_OP || op == GE_OP)
                                continue;
                        } else if (flag < 0) {
                            if (op == LT_OP || op == LE_OP || op == NE_OP)
                                continue;
                        } else {
                            if (op == GT_OP || op == GE_OP || op == NE_OP)
                                continue;
                        }
                    } else {
                        err << "[ERROR] strange, no valid situation" << endl;
                        return;
                    }
                    pass = 0;
                    break;
                }
                if (pass) {
                    cnt ++;
                    if (rhsCol >= 0) {
                        if (dataAttrInfo[rhsCol].attrType == INT) {
                            int rrv = *(int*)(r.getData() + dataAttrInfo[rhsCol].offset);
                            *(int*)(r.getData() + dataAttrInfo[writeBackCol].offset) = rrv;
                        } else if (dataAttrInfo[rhsCol].attrType == STRING) {
                            char *rrv = r.getData() + dataAttrInfo[rhsCol].offset;
                            strncpy(r.getData() + dataAttrInfo[writeBackCol].offset,
                                    rrv,
                                    min(dataAttrInfo[rhsCol].attrLength, dataAttrInfo[writeBackCol].attrLength)
                            );
                        } else {
                            err << "[ERROR] strange, no valid situation" << endl;
                            return;
                        }
                    } else {
                        if (dataAttrInfo[writeBackCol].attrType == INT) {
                            *(int*)(r.getData() + dataAttrInfo[writeBackCol].offset) = *(int*)(writeBackValue.data);
                        } else if (dataAttrInfo[writeBackCol].attrType == STRING) {
                            strncpy(r.getData() + dataAttrInfo[writeBackCol].offset,
                                    (char*)writeBackValue.data,
                                    dataAttrInfo[writeBackCol].attrLength
                            );
                        } else {
                            err << "[ERROR] strange, no valid situation" << endl;
                            return;
                        }
                    }
                    dbFile.updateRec(r);
                }
            }
            printer << "<UPDATE> " << cnt << " row(s)" << endl;
        }

        void executeNoJoinSelect(NODE *n) {


            int marker = -1;
            relName = n->u.QUERY.rellist->u.LIST.curr->u.RELATION.relname;
            if (!dbHandle->findTable(relName)) {
                err << "[ERROR] relation not found" << endl;
                return;
            }
            rm::FileHandle dbFile = dbHandle->openRelFile(relName);
            sm::RelInfo relInfo = dbHandle->getRelInfo(relName);
            memset(dataAttrInfo, 0, sizeof(dataAttrInfo));
            int attrNum = dbHandle->loadRelAttrInfo(relName, dataAttrInfo);
            int targetCol = -1;
            CompOp op;
            Value v;
            void *vValue = nullptr;
            condsPtr = 0;

            memset(selected, 0, sizeof(selected));

            for (i = 0, iter = n->u.QUERY.relattrlist; iter != nullptr; iter = iter->u.LIST.next, i++) {
                curr = iter->u.LIST.curr;
                if (!strcmp(curr->u.RELATTR.attrname, "*")) {
                    for (int j = 0; j < attrNum; j++) {
                        selected[j] = 1;
                    }
                    break;
                }
                for (int j = 0; j < attrNum; j++) {
                    if (!strcmp(dataAttrInfo[j].attrName, curr->u.RELATTR.attrname)) {
                        selected[j] = 1;
                    }
                }
            }

            /* construct conds table */
            for (i = 0, iter = n->u.QUERY.conditionlist; iter != nullptr; iter = iter->u.LIST.next, i++) {
                curr = iter->u.LIST.curr;
                conds[condsPtr].op = curr->u.CONDITION.op;
                conds[condsPtr].rhsRelId = 0;
                conds[condsPtr].rhsAttrId = -1;
                conds[condsPtr].attrId = -1;

                char *attrName = curr->u.CONDITION.lhsRelattr->u.RELATTR.attrname;
                for (int j = 0; j < attrNum; j++)
                    if (!strcmp(dataAttrInfo[j].attrName, attrName)) {
                        conds[condsPtr].attrId = j;
                    }
                if (curr->u.CONDITION.rhsRelattr != nullptr) {
                    char *rhsAttrName = curr->u.CONDITION.rhsRelattr->u.RELATTR.attrname;
                    for (int j = 0; j < attrNum; j++) {
                        if (!strcmp(dataAttrInfo[j].attrName, rhsAttrName)) {
                            conds[condsPtr].rhsAttrId = j;
                        }
                    }
                }
                if (curr->u.CONDITION.rhsValue != nullptr) {
                    makeValue(curr->u.CONDITION.rhsValue, v);
                    conds[condsPtr].rhsValue = v.data;
                    conds[condsPtr].rhsValueType = v.type;
                } else {
                    conds[condsPtr].rhsValue = nullptr;
                    conds[condsPtr].rhsValueType = INT;
                }

                if ((curr->u.CONDITION.rhsRelattr != nullptr && conds[condsPtr].rhsAttrId < 0) || conds[condsPtr].attrId < 0) {
                    err << "[ERROR] unknown column" << endl;
                    return;
                }
                condsPtr++;
            }

            /* find first condition */
            for (int i = 0; i < condsPtr; i++) {
                if (conds[i].rhsAttrId == -1) {
                    marker = i;
                    break;
                }
            }

            if (marker < 0) {
                op = NO_OP;
                targetCol = 0;
            } else {
                targetCol = conds[marker].attrId;
                op = conds[marker].op;
            }

            rm::FileScan dbScan(
                    dbFile,
                    dataAttrInfo[targetCol].attrType,
                    dataAttrInfo[targetCol].attrLength,
                    dataAttrInfo[targetCol].offset,
                    dataAttrInfo[targetCol].nullBitOffset,
                    dataAttrInfo[targetCol].nullBitMask,
                    op,
                    conds[marker].rhsValue
            );

            int cnt = 0;
            while (true) {
                Record &r = dbScan.next();
                if (r.rid.pageNum <= 0) break;
                /* check other contraints */
                int pass = 1;
                for (int i = 0; i < condsPtr; i++) {
                    if (i == marker) continue;
                    op = conds[i].op;
                    sm::DataAttrInfo &lhsInfo = dataAttrInfo[conds[i].attrId];
                    if (op == NO_OP) {
                        continue;
                    } else if (op == IS_NULL_OP) {
                        if (lhsInfo.nullable && ((*(r.getData() + lhsInfo.nullBitOffset)) & lhsInfo.nullBitMask))
                            continue;
                    } else if (op == NOT_NULL_OP) {
                        if (!lhsInfo.nullable || !((*(r.getData() + lhsInfo.nullBitOffset)) & lhsInfo.nullBitMask))
                            continue;
                    } else if (conds[i].rhsAttrId >= 0) { // comps between to col
                        sm::DataAttrInfo &rhsInfo = dataAttrInfo[conds[i].rhsAttrId];
                        if (lhsInfo.attrType != rhsInfo.attrType) {
                            err << "[ERROR] uncomparable attributes" << endl;
                            return;
                        }
                        int flag;
                        if (lhsInfo.attrType == INT) {
                            int llv = *(int *)(r.getData() + lhsInfo.offset);
                            int rrv = *(int *)(r.getData() + rhsInfo.offset);
                            flag = llv < rrv ? -1 : llv > rrv ? 1 : 0;
                        } else {
                            char *llv = r.getData() + lhsInfo.offset;
                            char *rrv = r.getData() + rhsInfo.offset;
                            flag = strncmp(llv, rrv, max(lhsInfo.attrLength, rhsInfo.attrLength));
                        }
                        if (flag == 0) {
                            if (op == EQ_OP || op == LE_OP || op == GE_OP)
                                continue;
                        } else if (flag < 0) {
                            if (op == LT_OP || op == LE_OP || op == NE_OP)
                                continue;
                        } else {
                            if (op == GT_OP || op == GE_OP || op == NE_OP)
                                continue;
                        }

                    } else if (conds[i].rhsValue != nullptr) {
                        if (lhsInfo.attrType != conds[i].rhsValueType) {
                            err << "[ERROR] uncomparable attributes" << endl;
                            return;
                        }
                        int flag;
                        if (lhsInfo.attrType == INT) {
                            int llv = *(int *)(r.getData() + lhsInfo.offset);
                            int rrv = *(int *)(conds[i].rhsValue);
                            flag = llv < rrv ? -1 : llv > rrv ? 1 : 0;
                        } else {
                            char *llv = r.getData() + lhsInfo.offset;
                            char *rrv = (char*)(conds[i].rhsValue);
                            flag = strncmp(llv, rrv, lhsInfo.attrLength);
                        }
                        if (flag == 0) {
                            if (op == EQ_OP || op == LE_OP || op == GE_OP)
                                continue;
                        } else if (flag < 0) {
                            if (op == LT_OP || op == LE_OP || op == NE_OP)
                                continue;
                        } else {
                            if (op == GT_OP || op == GE_OP || op == NE_OP)
                                continue;
                        }
                    } else {
                        err << "[ERROR] strange, no valid situation" << endl;
                        return;
                    }
                    pass = 0;
                    break;
                }
                if (pass) {
                    cnt ++;
                    for (int j = 0; j < attrNum; j++) {
                        if (selected[j]) {
                            printer << "";
                            switch (dataAttrInfo[j].attrType) {
                                case INT:
                                    printer << *(int *) (r.getData() + dataAttrInfo[j].offset);
                                    break;
                                case STRING:
                                    printer << r.getData() + dataAttrInfo[j].offset;
                                    break;
                                default:
                                    printer << *(real *) (r.getData() + dataAttrInfo[j].offset);
                                    break;
                            }
                            printer << "\t";
                        }
                    }
                    printer << "" << endl;
                }
            }
            printer << "<SELECT> " << cnt << " rows" << endl;
        }

        bool decideJoinOrder(NODE *n){

            double dataAttrInfo; // ... prevent bugs
            string attrNum; // ...
            Value v;
            int start = -1;

            for (i = 0; i < relNum; i++) {
                attrCnt[i] = dbHandle->loadRelAttrInfo(relNames[i], manyDataAttrInfo[i]);
                manyRelInfo[i] = dbHandle->getRelInfo(relNames[i]);
            }
            condsPtr = 0;
            memset(conds, 0, sizeof(conds));
            /* construct conds table */
            for (i = 0, iter = n->u.QUERY.conditionlist; iter != nullptr; iter = iter->u.LIST.next, i++) {
                curr = iter->u.LIST.curr;
                conds[condsPtr].op = curr->u.CONDITION.op;
                conds[condsPtr].relId = -1;
                conds[condsPtr].rhsRelId = -1;
                conds[condsPtr].rhsAttrId = -1;
                conds[condsPtr].attrId = -1;

                char *attrName = curr->u.CONDITION.lhsRelattr->u.RELATTR.attrname;
                char *relName = curr->u.CONDITION.lhsRelattr->u.RELATTR.relname;
                if (curr->u.CONDITION.lhsRelattr->u.RELATTR.relname != nullptr) {
                    for (int j = 0; j < relNum; j++)
                        if (!strcmp(relNames[j], relName)) {
                            conds[condsPtr].relId = j;
                        }
                    if (conds[condsPtr].relId < 0) {
                        err << "[ERROR] invalid rel name" << endl;
                        return false;
                    }
                } else {
                    err << "[ERROR] need rel name" << endl;
                    return false;
                }
                for (int j = 0; j < attrCnt[conds[condsPtr].relId]; j++)
                    if (!strcmp(manyDataAttrInfo[conds[condsPtr].relId][j].attrName, attrName)) {
                        conds[condsPtr].attrId = j;
                    }
                if (curr->u.CONDITION.rhsRelattr != nullptr) {
                    char *rhsRelName = curr->u.CONDITION.rhsRelattr->u.RELATTR.relname;
                    if (rhsRelName != nullptr) {
                        for (int j = 0; j < relNum; j++)
                            if (!strcmp(relNames[j], rhsRelName)) {
                                conds[condsPtr].rhsRelId = j;
                            }
                    } else {
                        err << "[ERROR] need rel name" << endl;
                        return false;
                    }
                    if (conds[condsPtr].rhsRelId < 0) {
                        err << "[ERROR] invalid rel name" << endl;
                        return false;
                    }
                    char *rhsAttrName = curr->u.CONDITION.rhsRelattr->u.RELATTR.attrname;
                    for (int j = 0; j < attrCnt[conds[condsPtr].rhsRelId]; j++) {
                        if (!strcmp(manyDataAttrInfo[conds[condsPtr].rhsRelId][j].attrName, rhsAttrName)) {
                            conds[condsPtr].rhsAttrId = j;
                        }
                    }
                }
                if (conds[condsPtr].relId != conds[condsPtr].rhsRelId) start = i;
                if (curr->u.CONDITION.rhsValue != nullptr) {
                    makeValue(curr->u.CONDITION.rhsValue, v);
                    if (v.type != manyDataAttrInfo[conds[condsPtr].relId][conds[condsPtr].attrId].attrType) {
                        err << "[ERROR] type dismatch" << endl;
                        return false;
                    }
                    conds[condsPtr].rhsValue = v.data;
                    conds[condsPtr].rhsValueType = v.type;
                } else {
                    if (manyDataAttrInfo[conds[condsPtr].relId][conds[condsPtr].attrId].attrType !=
                            manyDataAttrInfo[conds[condsPtr].rhsRelId][conds[condsPtr].rhsAttrId].attrType) {
                        err << "[ERROR] type dismatch" << endl;
                        return false;
                    }
                    conds[condsPtr].rhsValue = nullptr;
                    conds[condsPtr].rhsValueType = INT;
                }

                if ((curr->u.CONDITION.rhsRelattr != nullptr && conds[condsPtr].rhsAttrId < 0) || conds[condsPtr].attrId < 0) {
                    err << "[ERROR] unknown column" << endl;
                    return false;
                }
                condsPtr++;
            }

            if (start < 0) { // so bad...
                conds[condsPtr].relId = 0;
                conds[condsPtr].attrId = 0;
                conds[condsPtr].rhsAttrId = -1;
                conds[condsPtr].rhsRelId = -1;
                conds[condsPtr].op = NO_OP;
                conds[condsPtr].rhsValue = nullptr;
                conds[condsPtr].rhsValueType = INT;
                start = condsPtr;
                condsPtr++;
            }
            for (i = 0; i < condsPtr; i++) queryOrder[i] = -1;
            queryOrder[0] = start;
            memset(condFlag, 0, sizeof(condFlag));
            condFlag[start] = 1;


            for (i = 1; i < condsPtr; i++) {
                queryOrder[i] = -1;
                int b = queryOrder[i - 1];
                for (int j = 0; j < condsPtr; j++) {
                    if (!condFlag[j]) {
                        if (conds[j].relId == conds[b].relId) {
                            joinPoint[i][0] = 0;
                            joinPoint[i][1] = 0;
                            queryOrder[i] = j;
                            condFlag[j] = 1;
                            break;
                        } else if (conds[j].relId == conds[b].rhsRelId) {
                            joinPoint[i][0] = 0;
                            joinPoint[i][1] = 1;
                            queryOrder[i] = j;
                            condFlag[j] = 1;
                            break;
                        } else if (conds[j].rhsRelId == conds[b].relId) {
                            joinPoint[i][0] = 1;
                            joinPoint[i][1] = 0;
                            queryOrder[i] = j;
                            condFlag[j] = 1;
                            break;
                        } else if (conds[j].rhsRelId == conds[b].rhsRelId) {
                            joinPoint[i][0] = 1;
                            joinPoint[i][1] = 1;
                            queryOrder[i] = j;
                            condFlag[j] = 1;
                            break;
                        }
                    }
                }
                if (queryOrder[i] == -1)
                    return false;
            }

            return true;
        }

        void printSelected(vector<RID> &selected) {
            for (int i = 0; i < selected.size(); i++) {
                int relId = selected[i].pageNum;
                int attrId = selected[i].slotNum;
                Record &r = *(joined[relId]);
                sm::DataAttrInfo &dai = manyDataAttrInfo[relId][attrId];
                printer << "";
                switch (dai.attrType) {
                    case INT:
                        printer << *(int *) (r.getData() + dai.offset);
                        break;
                    case STRING:
                        printer << r.getData() + dai.offset;
                        break;
                    default:
                        printer << *(real *) (r.getData() + dai.offset);
                        break;
                }
                printer << "\t";
            }
            printer << endl;
            selectCnt += 1;
        }

        void recursiveJoin(int nowPtr, vector<RID> &selected) {
            if (nowPtr == condsPtr) {
                printSelected(selected);
                return;
            }
            CondSymbol &c = conds[queryOrder[nowPtr]];
            bool iHaveLhs = (joined[c.relId] != nullptr);
            bool iHaveRhs = (c.rhsRelId >= 0 && joined[c.rhsRelId] != nullptr);
            bool valuedRhs = (c.rhsAttrId < 0);
            if (!iHaveLhs && !iHaveRhs && !valuedRhs) {
                err << "[NOT IMPL] complex query plan" << endl;
                return;
            }
            rm::FileScan *dbScan = nullptr;
            ix::Index *index = nullptr;
            char *tmpValue;
            if (!iHaveLhs && valuedRhs) {
                sm::DataAttrInfo &dai = manyDataAttrInfo[c.relId][c.attrId];
                if (dai.indexNo < 0 || c.op != EQ_OP) {
                    dbScan = new rm::FileScan(
                            *dbFiles[c.relId],
                            dai.attrType,
                            dai.attrLength,
                            dai.offset,
                            dai.nullBitOffset,
                            dai.nullBitMask,
                            c.op,
                            c.rhsValue
                    );
                } else {
                    index = new ix::Index(sm::SystemManager::appendIndexExt(relNames[c.relId], dai.indexNo).c_str());
                    RID rid = index->searchEntry(*(int*)c.rhsValue);
                    if (rid.pageNum > 0) {
                        joined[c.relId] = new Record(manyRelInfo[c.relId].tupleLength + manyRelInfo[c.relId].bitmapSize + FLAG_SIZE);
                        dbFiles[c.relId]->getRec(rid, *joined[c.relId]);
                        needDel[c.relId] = 1;
                        recursiveJoin(nowPtr + 1, selected);
                    }
                    delete index;
                    if (joined[c.relId] != nullptr) delete joined[c.relId];
                    joined[c.relId] = nullptr;
                    if (tmpValue != nullptr) delete[] tmpValue;
                    return;
                }
            }
            if (!iHaveLhs && iHaveRhs && !valuedRhs) { // is a joined record
                Record *rhr = joined[c.rhsRelId];
                sm::DataAttrInfo &dai = manyDataAttrInfo[c.relId][c.attrId];
                sm::DataAttrInfo &dai2 = manyDataAttrInfo[c.rhsRelId][c.rhsAttrId];
                tmpValue = new char[dai2.attrLength + 1];
                memset(tmpValue, 0, dai2.attrLength + 1);
                memcpy(tmpValue, rhr->getData() + dai2.offset, dai2.attrLength);
                if (dai.indexNo < 0 || c.op != EQ_OP) {
                    dbScan = new rm::FileScan(
                            *dbFiles[c.relId],
                            dai.attrType,
                            dai.attrLength,
                            dai.offset,
                            dai.nullBitOffset,
                            dai.nullBitMask,
                            c.op,
                            tmpValue
                    );
                } else {
                    index = new ix::Index(sm::SystemManager::appendIndexExt(relNames[c.relId], dai.indexNo).c_str());
                    RID rid = index->searchEntry(*(int*)tmpValue);
                    if (rid.pageNum > 0) {
                        joined[c.relId] = new Record(manyRelInfo[c.relId].tupleLength + manyRelInfo[c.relId].bitmapSize + FLAG_SIZE);
                        dbFiles[c.relId]->getRec(rid, *joined[c.relId]);
                        needDel[c.relId] = 1;
                        recursiveJoin(nowPtr + 1, selected);
                    }
                    delete index;
                    if (joined[c.relId] != nullptr) delete joined[c.relId];
                    joined[c.relId] = nullptr;
                    if (tmpValue != nullptr) delete[] tmpValue;
                    return;
                }
            }
            if (iHaveLhs && !iHaveRhs) {
                Record *lhr = joined[c.relId];
                sm::DataAttrInfo &dai = manyDataAttrInfo[c.rhsRelId][c.rhsAttrId];
                sm::DataAttrInfo &dai2 = manyDataAttrInfo[c.relId][c.attrId];
                tmpValue = new char[dai2.attrLength + 1];
                memset(tmpValue, 0, dai2.attrLength + 1);
                memcpy(tmpValue, lhr->getData() + dai2.offset, dai2.attrLength);
                if (dai.indexNo < 0 || c.op != EQ_OP) {
                    dbScan = new rm::FileScan(
                            *dbFiles[c.rhsRelId],
                            dai.attrType,
                            dai.attrLength,
                            dai.offset,
                            dai.nullBitOffset,
                            dai.nullBitMask,
                            c.op,
                            tmpValue
                    );
                } else {
                    index = new ix::Index(sm::SystemManager::appendIndexExt(relNames[c.rhsRelId], dai.indexNo).c_str());
                    RID rid = index->searchEntry(*(int*)tmpValue);
                    if (rid.pageNum > 0) {
                        joined[c.rhsRelId] = new Record(manyRelInfo[c.rhsRelId].tupleLength + manyRelInfo[c.rhsRelId].bitmapSize + FLAG_SIZE);
                        dbFiles[c.rhsRelId]->getRec(rid, *joined[c.rhsRelId]);
                        needDel[c.rhsRelId] = 1;
                        recursiveJoin(nowPtr + 1, selected);
                    }
                    delete index;
                    if (joined[c.rhsRelId] != nullptr) delete joined[c.rhsRelId];
                    joined[c.rhsRelId] = nullptr;
                    if (tmpValue != nullptr) delete[] tmpValue;
                    return;
                }
            }
            if (iHaveLhs && iHaveRhs) {
                Record *lhr = joined[c.relId];
                Record *rhr = joined[c.rhsRelId];
                sm::DataAttrInfo &rhsInfo = manyDataAttrInfo[c.rhsRelId][c.rhsAttrId];
                sm::DataAttrInfo &lhsInfo = manyDataAttrInfo[c.relId][c.attrId];
                int flag;
                if (lhsInfo.attrType == INT) {
                    int llv = *(int *)(lhr->getData() + lhsInfo.offset);
                    int rrv = *(int *)(rhr->getData() + rhsInfo.offset);
                    flag = llv < rrv ? -1 : llv > rrv ? 1 : 0;
                } else {
                    char *llv = lhr->getData() + lhsInfo.offset;
                    char *rrv = rhr->getData() + rhsInfo.offset;
                    flag = strncmp(llv, rrv, max(lhsInfo.attrLength, rhsInfo.attrLength));
                }
                if (flag == 0) {
                    if (c.op == EQ_OP || c.op == LE_OP || c.op == GE_OP)
                        recursiveJoin(nowPtr + 1, selected);
                } else if (flag < 0) {
                    if (c.op == LT_OP || c.op == LE_OP || c.op == NE_OP)
                        recursiveJoin(nowPtr + 1, selected);
                } else {
                    if (c.op == GT_OP || c.op == GE_OP || c.op == NE_OP)
                        recursiveJoin(nowPtr + 1, selected);
                }
                return;
            }
            if (iHaveLhs && valuedRhs) {
                int flag;
                Record *lhr = joined[c.relId];
                sm::DataAttrInfo &lhsInfo = manyDataAttrInfo[c.relId][c.attrId];
                if (lhsInfo.attrType == INT) {
                    int llv = *(int *)(lhr->getData() + lhsInfo.offset);
                    int rrv = *(int *)(c.rhsValue);
                    flag = llv < rrv ? -1 : llv > rrv ? 1 : 0;
                } else {
                    char *llv = lhr->getData() + lhsInfo.offset;
                    char *rrv = (char*)(c.rhsValue);
                    flag = strncmp(llv, rrv, lhsInfo.attrLength);
                }
                if (flag == 0) {
                    if (c.op == EQ_OP || c.op == LE_OP || c.op == GE_OP)
                        recursiveJoin(nowPtr + 1, selected);
                } else if (flag < 0) {
                    if (c.op == LT_OP || c.op == LE_OP || c.op == NE_OP)
                        recursiveJoin(nowPtr + 1, selected);
                } else {
                    if (c.op == GT_OP || c.op == GE_OP || c.op == NE_OP)
                        recursiveJoin(nowPtr + 1, selected);
                }
                return;
            }

            if (dbScan != nullptr) {
                while (true) {
                    int relId = iHaveRhs ? c.relId : c.rhsRelId;
                    int attrId = iHaveRhs ? c.attrId : c.rhsAttrId;
                    Record &rrr = dbScan->next();
                    Record *r = joined[relId] = dbScan->current();
                    if (r->rid.pageNum <= 0) {
                        joined[relId] = nullptr;
                        break;
                    }
                    recursiveJoin(nowPtr + 1, selected);
                    joined[relId] = nullptr;
                }
                delete dbScan;
            }

        }

        void execute2JoinSelect(NODE *n) {

            int marker = -1;

            for (i = 0, iter = n->u.QUERY.rellist; iter != nullptr; iter = iter->u.LIST.next, i++) {
                curr = iter->u.LIST.curr;
                char *relName = curr->u.RELATION.relname;
                if (!dbHandle->findTable(relName)) {
                    err << "[ERROR] relation not found" << endl;
                    return;
                }
                relNames[i] = relName;
            }
            relNum = i;

            if (!decideJoinOrder(n)) {
                err << "[ERROR] query planning failed" << endl;
                return;
            }

            string dataAttrInfo; //...
            char *relName = relNames[queryOrder[0]];
            int targetCol = -1;
            CompOp op;
            Value v;
            void *vValue = nullptr;

            using namespace std;
            vector < RID > selected;

            for (i = 0, iter = n->u.QUERY.relattrlist; iter != nullptr; iter = iter->u.LIST.next, i++) {
                curr = iter->u.LIST.curr;

                char *attrName = curr->u.RELATTR.attrname;
                char *relName = curr->u.RELATTR.relname;
                if (attrName == nullptr || relName == nullptr) {
                    err << "[ERROR] bad rel/attr name" << endl;
                    return;
                }
                int flag = 0;
                for (int j = 0; j < relNum; j++) {
                    for (int k = 0; k < attrCnt[j]; k++)
                        if (!strcmp(relName, manyDataAttrInfo[j][k].relName) &&
                                !strcmp(attrName, manyDataAttrInfo[j][k].attrName)) {

                            flag = 1;
                            selected.push_back(RID(j, k));
                            break;
                        }
                }
                if (flag == 0) {
                    err << "[ERROR] bad rel/attr name" << endl;
                    return;
                }
            }


            CondSymbol &c = conds[queryOrder[0]];
            sm::DataAttrInfo &dai = manyDataAttrInfo[c.relId][c.attrId];
            string dbFile; // prevent bug;
            for (int i = 0; i < relNum; i++) {
                dbFiles[i] = new rm::FileHandle();
                *dbFiles[i] = dbHandle->openRelFile(relNames[i]);
            }

            rm::FileScan dbScan(
                    *dbFiles[c.relId],
                    dai.attrType,
                    dai.attrLength,
                    dai.offset,
                    dai.nullBitOffset,
                    dai.nullBitMask,
                    c.op,
                    c.rhsValue
            );

            for (int i = 0; i < relNum; i++)
                joined[i] = nullptr;

            selectCnt = 0;
            while (true) {
                Record &r = dbScan.next();
                if (r.rid.pageNum <= 0) break;
                joined[c.relId] = dbScan.current();
                recursiveJoin(1, selected);
            }
            for (int i = 0; i < relNum; i++) {
                delete dbFiles[i];
                dbFiles[i] = nullptr;
            }
            printer << "<SELECT> " << selectCnt << " rows" << endl;

        }

        void interp(NODE *n) {

            using namespace std;

            if (dbHandle == nullptr &&
                    n->kind != N_CREATEDATABASE &&
                    n->kind != N_DROPDATABASE &&
                    n->kind != N_USEDATABASE
            ) {
                err << "[ERROR] SQLeast is not connected to a database yet" << endl;
                return;
            }

            switch (n->kind) {

                case N_CREATEDATABASE:
                    try {
                        if (dbHandle != nullptr) {
                            sm::SystemManager::closeDB();
                            delete dbHandle;
                            dbHandle = nullptr;
                        }
                        sm::SystemManager::createDB(n->u.CREATEDATABASE.dbname);
                        break;
                    } catch (sm::SMException e) {
                        err << "[ERROR] DB already exists" << endl;
                        return;
                    }
                    break;

                case N_DROPDATABASE:
                    try {
                        if (dbHandle != nullptr) {
                            sm::SystemManager::closeDB();
                            delete dbHandle;
                            dbHandle = nullptr;
                        }
                        sm::SystemManager::destroyDB(n->u.DROPDATABASE.dbname);
                    } catch (sm::SMException e) {
                        err << "[ERROR] no such DB" << endl;
                        return;
                    }
                    break;

                case N_USEDATABASE:
                    try {
                        sm::SystemManager::useDB(n->u.USEDATABASE.dbname);
                        if (dbHandle != nullptr) delete dbHandle;
                        dbHandle = new sm::DBHandle(n->u.USEDATABASE.dbname);
                    } catch (sm::SMException e) {
                        err << "[ERROR] no such DB" << endl;
                        return;
                    }
                    break;

                case N_SHOWTABLES:
                    dbHandle->showTables();
                    break;

                case N_DESCTABLE:
                    dbHandle->descTable(n->u.DESCTABLE.relname);
                    break;

                case N_CREATETABLE:            /* for CreateTable() */
                    relName = n->u.CREATETABLE.relname;
                    if (dbHandle->findTable(relName)) {
                        err << "[ERROR] Relation exists" << endl;
                        return;
                    }
                    primary = n->u.CREATETABLE.primary;
                    attrNum = 0;
                    memset(attrInfo, 0, sizeof(attrInfo));
                    for (iter = n->u.CREATETABLE.attrlist; iter != nullptr; iter = iter->u.LIST.next) {
                        curr = iter->u.LIST.curr;
                        if (curr->kind == N_ATTRPROP) continue;
                        attrInfo[attrNum].attrName = curr->u.ATTRTYPE.attrname;
                        attrInfo[attrNum].attrLength = curr->u.ATTRTYPE.length;
                        attrInfo[attrNum].nullable = curr->u.ATTRTYPE.nullable;
                        char *type = curr->u.ATTRTYPE.type;
                        if (lower_equal(type, "int")) {
                            attrInfo[attrNum].attrType = INT;
                        } else if (lower_equal(type, "varchar")) {
                            attrInfo[attrNum].attrType = STRING;
                        } else if (lower_equal(type, "float")) {
                            attrInfo[attrNum].attrType = FLOAT;
                        } else {
                            err << "[ERROR] invalid type" << endl;
                            return;
                        }
                        attrNum += 1;
                    }
                    dbHandle->createTable(relName, attrNum, primary, attrInfo);
                    break;

                case N_CREATEINDEX:            /* for CreateIndex() */
                    printer << "[NOT IMPL] create index" << endl;
                    break;

                case N_DROPINDEX:            /* for DropIndex() */
                    printer << "[NOT IMPL] drop index" << endl;
                    break;

                case N_DROPTABLE:            /* for DropTable() */
                    dbHandle->dropTable(n->u.DROPTABLE.relname);
                    break;

                case N_QUERY:            /* for Query() */
                    for (iter = n->u.QUERY.rellist, i = 0; iter != nullptr; iter = iter->u.LIST.next, i++);
                    if (i == 1)
                        executeNoJoinSelect(n);
                    else
                        execute2JoinSelect(n);
                    break;

                case N_INSERT:            /* for Insert() */
                    executeInsert(n);
                    break;

                case N_DELETE:            /* for Delete() */
                    executeDelete(n);
                    break;

                case N_UPDATE:            /* for Update() */
                    executeUpdate(n);
                    break;

                default:   // should never get here
                    printer << "[UNSUPPORTED] This command is recognized but it won't be supported in SQLeast." << endl;
                    break;
            }
        }
    }
}