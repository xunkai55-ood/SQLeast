#ifndef IX_INDEX_H
#define IX_INDEX_H

#include "sqleast.h"

namespace sqleast {
    namespace ix {

        const int B_PLUS_TREE_BRANCH = 4; // k's max value when not leaf node

        struct Node {
            bool isLeaf;
            int size; // if leaf then size of n and k; if not then size of k;
            RID n[B_PLUS_TREE_BRANCH + 1];
            int k[B_PLUS_TREE_BRANCH + 1];
            RID parent;

            int getPosition(int value);
            bool insertN(RID value, int position);
            bool insertK(int value, int position);
            bool removeN(int position);
            bool removeK(int position);
        };

        class Index {

        public:
            Index(const char *indexName);

            RID insertEntry(int key, RID value);
            RID searchEntry(int key);

        private:
            FileHandle handle_;
            RID hot_;

            // TODO xk part
            RID getRootRID();
            void setRoot(RID rid);
            Node *getNode(RID rid);
            RID allocateNode();
            RID releaseNode(RID rid);
            void forcePages();
            int getIndexSize();
            void incIndexSize();
            void decIndexSize();
            // TODO yy part
            Node *getRoot();
            void solveOverFlow(RID rid);
            void solveUnderFlow(RID rid);
        };
    }
}


#endif