#ifndef RM_FILESCAN_H
#define RM_FILESCAN_H

#include "filehandle.h"

namespace sqleast {
    namespace rm {

        class FileScan {

        public:
            FileScan(FileHandle &handle, AttrType attrType, int attrLength, int attrOffset, CompOp compOp, void *value);
            ~FileScan();

            Record next();

        private:
            int pageNum_, slotNum_;

        };

    }
}

#endif