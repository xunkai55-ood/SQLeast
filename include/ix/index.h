#ifndef IX_INDEX_H
#define IX_INDEX_H

#include "sqleast.h"
#include "rm/filehandle.h"

namespace sqleast {
    namespace ix {

        const int B_PLUS_TREE_BRANCH = 4; // k's max value when not leaf node

        struct Node {
            bool isLeaf;
            int size; // if leaf then size of n and k; if not then size of k;
            RID parent;
            RID n[B_PLUS_TREE_BRANCH + 1];
            int k[B_PLUS_TREE_BRANCH + 1];

            int getPosition(int value);
            bool insertN(RID value, int position);
            bool insertK(int value, int position);
            bool removeN(int position);
            bool removeK(int position);
        };

        struct IndexInfo {
            int indexSize;
            int rootPageNum;
            int rootSlotNum;
        };

        class Index {

        public:
            Index(const char *indexName);
            ~Index();

            RID insertEntry(int key, RID value);
            RID searchEntry(int key);
            bool removeEntry(int key);

        private:
            rm::FileHandle handle_;
            RID hot_;
            IndexInfo indexInfo_;

            RID getRootRID();
            void setRoot(RID rid);
            void getNode(RID rid, Node &node);  // client firstly create a node instance, then pass it to the "get node" method
            void commitNode(RID rid, const Node &node);  // after a part of "write" operation, call this function to write to disk
            RID allocateNode();
            RID releaseNode(RID rid);
            void forcePages();
            int getIndexSize();
            void incIndexSize();
            void decIndexSize();
            void commitIndexInfo();

            // TODO yy part
            void getRoot(Node &node);
            void solveOverFlow(RID rid);
            void solveUnderFlow(RID rid);
        };
    }
}


#endif