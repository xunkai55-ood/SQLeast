#include "rm/filescan.h"
#include "rm/filehandle.h"
#include "rm/exception.h"

namespace sqleast {
    namespace rm {

        FileScan::FileScan(FileHandle &handle,
                AttrType attrType, int attrLength, int attrOffset,
                int nullBitOffset, int nullBitMask,
                CompOp compOp, void *value) :
        handle_(handle), attrType_(attrType), attrLength_(attrLength), attrOffset_(attrOffset),
        nullBitOffset_(nullBitOffset), nullBitMask_(nullBitMask), value_(value), compOp_(compOp),
        pageNum_(1), slotNum_(0), info_(handle.getInfo()), c_(info_.recordSize)
        {
        }

        FileScan::~FileScan() {
        }

        Record &FileScan::next() {
            do {
                if (pageNum_ == info_.totalPageNum) {
                    c_.rid.pageNum = -1;
                    c_.rid.slotNum = -1;
                    return c_;
                }
                handle_.getRec(RID(pageNum_, slotNum_), c_);
                if (c_.getFlag() & REC_ALIVE) {
                    if (compOp_ == NO_OP) {
                        break;
                    } else if (compOp_ == IS_NULL_OP) {
                        if ((*(c_.getData() + nullBitOffset_)) & nullBitMask_)
                            break;
                    } else if (compOp_ == NOT_NULL_OP) {
                        if (((*(c_.getData() + nullBitOffset_)) & nullBitMask_) == 0)
                            break;
                    } else if (attrType_ == INT) {
                        int attr = *(c_.getData() + attrOffset_);
                        int flag = 0;
                        // NO_OP, EQ_OP, LT_OP, GT_OP, LE_OP, GE_OP, NE_OP, IS_NULL_OP, NOT_NULL_OP
                        switch (compOp_) {
                            case EQ_OP: flag = (*(int*)value_ == attr); break;
                            case LT_OP: flag = (*(int*)value_ > attr); break;
                            case GT_OP: flag = (*(int*)value_ < attr); break;
                            case LE_OP: flag = (*(int*)value_ >= attr); break;
                            case GE_OP: flag = (*(int*)value_ <= attr); break;
                            case NE_OP: flag = (*(int*)value_ != attr); break;
                            default: flag = 0;
                        }
                        if (flag) break;
                    } else if (attrType_ == STRING) {
                        char *attr = c_.getData() + attrOffset_;
                        char *value = (char *) value_;
                        int i = strncmp(attr, value, (size_t)attrLength_);
                        if (i == 0) {
                            if (compOp_ == EQ_OP || compOp_ == LE_OP || compOp_ == GE_OP)
                                break;
                        } else if (i < 0) {
                            if (compOp_ == LT_OP || compOp_ == LE_OP || compOp_ == NE_OP)
                                break;
                        } else {
                            if (compOp_ == GT_OP || compOp_ == GE_OP || compOp_ == NE_OP)
                                break;
                        }
                    }
                }
                slotNum_ += 1;
                if (slotNum_ == info_.slotPerPage) {
                    slotNum_ = 0;
                    pageNum_ += 1;
                }
            } while (true);
            slotNum_ += 1;
            if (slotNum_ == info_.slotPerPage) {
                slotNum_ = 0;
                pageNum_ += 1;
            }
            return c_;
        }
    }
}