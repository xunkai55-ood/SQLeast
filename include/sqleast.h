#ifndef SQLEAST_DEF_H
#define SQLEAST_DEF_H

#include "pagefs/pagefs.h"

namespace sqleast {

    /* Definitions

     */

    enum AttrType {INT, FLOAT, STRING};

    enum CompOp {NO_OP, EQ_OP, LT_OP, GT_OP, LE_OP, GE_OP, NE_OP, IS_NULL_OP, NOT_NULL_OP};

    /* File

     */

    typedef pagefs::FileId FileId;

    /* Record

     * A record is a combination of validation info, real data and null bitmap.
     * rec = INFO + DATA + NULLBITMAP
     * where INFO is a flag unsigned int.
     */

    struct RID {
        int pageNum;
        int slotNum;

        RID(const int _pageNum, const int _slotNum): pageNum(_pageNum), slotNum(_slotNum) {}
        bool operator==(const RID &b) {
            return pageNum == b.pageNum && slotNum == b.slotNum;
        }
    };

    struct Record {
        RID rid;
        char *rData;

        Record(const RID _rid, char* _rData): rid(_rid), rData(_rData) {}
        Record(): rid(RID(-1, -1)), rData(nullptr) {}
        Record &operator=(const Record &r) {
            rid = r.rid;
            rData = r.rData;
            return *this;
        }

        inline int getFlag() {
            return *(int*)rData;
        }
        inline char *getData() {
            return rData + sizeof(int);
        }
    };

    enum RecordFlags {
        REC_ALIVE = 1
    };


}


#endif