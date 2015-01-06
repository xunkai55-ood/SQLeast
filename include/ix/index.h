#ifndef IX_INDEX_H
#define IX_INDEX_H

#include "sqleast.h"

namespace sqleast {
    namespace ix {

        const int B_PLUS_TREE_BRANCH = 4;

        struct Node {
            bool isLeaf;
            int size;
            RID n[B_PLUS_TREE_BRANCH + 1];
            int k[B_PLUS_TREE_BRANCH + 1];
            RID parent;
        };

        class Index {

        public:
            Index(const char *indexName);

            // void insertEntry(int key, RID value);
            // void findEntry(int key);

        private:
            FileHandle handle_;

            Node *getNode(RID rid);
            RID allocateNode();
            RID releaseNode(RID rid);
            void forcePages();
        };
    }
}


#endif