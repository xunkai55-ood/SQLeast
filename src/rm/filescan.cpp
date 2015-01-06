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
        nullBitOffset_(nullBitOffset), nullBitMask_(nullBitMask), value_(value),
        pageNum_(1), slotNum_(0), info_(handle.getInfo())
        {
            // TODO
        }

        FileScan::~FileScan() {
            // TODO
        }

        Record FileScan::next() {
            if (pageNum_ == info_.totalPageNum) {
                throw EOFException();
            }
            Record c;
            do {
                c = handle_.getRec(RID(pageNum_, slotNum_));
            } while ((c.getFlag() & REC_ALIVE) == 0);
            slotNum_ += 1;
            if (slotNum_ == info_.slotPerPage) {
                slotNum_ = 0;
                pageNum_ += 1;
            }
            return c;
        }
    }
}