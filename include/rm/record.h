#ifndef RM_RECORD_H
#define RM_RECORD_H

#include "rid.h"

namespace sqleast {

    namespace rm {

        struct Record {
            RID rid;
            char *rData;

            Record(RID _rid, char* _rData): rid(_rid), rData(_rData) {}
        };

    }

}

#endif