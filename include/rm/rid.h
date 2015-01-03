#ifndef RM_RID_H
#define RM_RID_H

namespace sqleast {

    namespace rm {

        struct RID {
            int pageNum;
            int slotNum;

            RID(int _pageNum, int _slotNum): pageNum(_pageNum), slotNum(_slotNum) {}
        };

    }
}

#endif