#ifndef IX_INDEXSCAN_H
#define IX_INDEXSCAN_H

#include <vector>
#include "ix/index.h"

namespace sqleast {
    namespace ix {

        class IndexScan {
        public:
            IndexScan(Index& i, int v, CompOp c);
            ~IndexScan();
            RID next();
        private:
            Index &index_;
            int value_;
            CompOp compOp_;
            bool canDo;

            RID current;
            std::vector<int> position;

            void toStart1(); // for no, eq, ne
            void toStart2(); // for lt, gt, le, ge
            bool toRight();
            bool toLeft();
            RID NoSearch();
            RID EqSearch();
            RID LtSearch();
            RID GtSearch();
            RID LeSearch();
            RID GeSearch();
            RID NeSearch();
        };

    }
}

#endif