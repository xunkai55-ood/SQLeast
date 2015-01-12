#ifndef SQLEAST_DEF_H
#define SQLEAST_DEF_H

#define PATH_SEP "/"

#include "pagefs/pagefs.h"

namespace sqleast {

    /* Definitions

     */

    const int MAX_COND_NUM = 6;
    const int MAX_NAME_LENGTH = 20;
    const int MAX_PATH_LENGTH = 200;

    enum AttrType {INT, STRING, FLOAT, NULLV};

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

        inline void clear() {
            memset(rData, 0, size);
        }
    };

    enum RecordFlags {
        REC_ALIVE = 1
    };

    const int MAX_ATTR_NUM = 100;

    enum AttrProp {
        PRIMARY_KEY
    };

    struct AttrInfo {
        char *attrName;
        AttrType attrType;
        int attrLength;
        int nullable;
        bool isPrimary;
    };

    struct RelAttr{
        char     *relName;    // Relation name (may be NULL)
        char     *attrName;   // Attribute name

        // Print function
//    friend std::ostream &operator<<(std::ostream &s, const RelAttr &ra);
    };

    struct Value{
        AttrType type;         /* type of value               */
        void     *data;        /* value                       */
        /* print function              */
//    friend std::ostream &operator<<(std::ostream &s, const Value &v);
    };

    struct Condition{
        RelAttr  lhsAttr;    /* left-hand side attribute            */
        CompOp   op;         /* comparison operator                 */
        int      bRhsIsAttr; /* TRUE if the rhs is an attribute,    */
        /* in which case rhsAttr below is valid;*/
        /* otherwise, rhsValue below is valid.  */
        RelAttr  rhsAttr;    /* right-hand side attribute            */
        Value    rhsValue;   /* right-hand side value                */
        /* print function                               */
//    friend std::ostream &operator<<(std::ostream &s, const Condition &c);

    };

//std::ostream &operator<<(std::ostream &s, const CompOp &op);
//std::ostream &operator<<(std::ostream &s, const AttrType &at);

//
// Parse function
//

    void sqleast_parse();

//
// Error printing function; calls component-specific functions
//

// bQueryPlans is allocated by parse.y.  When bQueryPlans is 1 then the
// query plan chosen for the SFW query will be displayed.  When
// bQueryPlans is 0 then no query plan is shown.
    extern int bQueryPlans;

    /* utils */
    inline int lower_equal(const char *a, const char *b) {
        char t;
        while (*a && *b) {
            t = *a;
            if (*a < 'a') t += 'z' - 'Z';
            if (t != *b) return 0;
            a++;
            b++;
        }
        return (!*a && !*b);
    }

}


#endif