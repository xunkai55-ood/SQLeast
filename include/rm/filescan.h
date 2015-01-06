#ifndef RM_FILESCAN_H
#define RM_FILESCAN_H

#include "filehandle.h"

namespace sqleast {
    namespace rm {

        class FileScan {

        public:
            FileScan(FileHandle &handle,
                    AttrType attrType, int attrLength, int attrOffset,
                    int nullBitOffset, int nullBitMask,
                    CompOp compOp, void *value);
            ~FileScan();

            Record next();

        private:
            int pageNum_, slotNum_;
            AttrType attrType_;
            int attrLength_, attrOffset_, nullBitOffset_, nullBitMask_;
            CompOp compOp_;
            void *value_;
            FileHandle handle_;
            FileInfo info_;
        };

    }
}

#endif