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

    const size_t FLAG_SIZE = sizeof(int);

    struct RID {
        int pageNum;
        int slotNum;

        RID(): pageNum(-1), slotNum(-1) {}
        RID(const int _pageNum, const int _slotNum): pageNum(_pageNum), slotNum(_slotNum) {}
        bool operator==(const RID &b) {
            return pageNum == b.pageNum && slotNum == b.slotNum;
        }
    };

    struct Record {
        RID rid;
        char *rData;
        const size_t size;

        Record(int _size): size((size_t)_size) {
            rData = new char[size];
        }

        ~Record() {
            delete[] rData;
        }

        Record &operator=(const Record &r) {
            rid = r.rid;
            rData = r.rData;
            delete[] rData;
            memcpy(rData, r.rData, size);
            return *this;
        }

        inline int getFlag() {
            return *(int*)rData;
        }
        inline char *getData() {
            return rData + FLAG_SIZE;
        }
    };

    enum RecordFlags {
        REC_ALIVE = 1
    };


}


#endif