#include <AppKit/AppKit.h>
#include "ix/index.h"
#include "rm/filehandle.h"

namespace sqleast {
    namespace ix {

        int Node::getPosition(int value) {
            int lo = 0;
            int hi = size;
            int mi;
            while(lo < hi){
                mi = (lo + hi) / 2;
                if(k[mi] == value)
                    return mi;
                if(k[mi] < value)
                    lo = mi + 1;
                else
                    hi = mi;
            }
            return lo - 1;
        }

        bool Node::insertK(int value, int position) {
            if(isLeaf && size == B_PLUS_TREE_BRANCH + 1)
                return false;
            if(!isLeaf && size == B_PLUS_TREE_BRANCH)
                return false;
            size ++;
            for(int i = B_PLUS_TREE_BRANCH ; i > position ; i --)
                k[i] = k[i-1];
            k[position] = value;
            return true;
        }

        bool Node::insertN(RID value, int position) {
            if(size == B_PLUS_TREE_BRANCH + 1)
                return false;
            for(int i = B_PLUS_TREE_BRANCH ; i > position ; i --)
                n[i] = n[i-1];
            n[position] = value;
            return true;
        }

        bool Node::removeK(int position) {
            if(size == 0)
                return false;
            size --;
            for(int i = position ; i < B_PLUS_TREE_BRANCH ; i ++)
                k[i] = k[i+1];
            return true;
        }

        bool Node::removeN(int position) {
            if(size == 0)
                return false;
            for(int i = position ; i < B_PLUS_TREE_BRANCH ; i ++)
                n[i] = n[i+1];
            return true;
        }

        /* index */

        Index::Index(const char *indexName):
                handle_(rm::RecordManager::openFile(indexName))
        {
            try {
                rm::RecordManager::createFile(indexName, sizeof(Node), false);
            } catch (pagefs::FileExistsException) {
                //
            }
            hot_ = RID(-1, -1);
            char *p = handle_.getFileInfo();
            p += sizeof(rm::FileHandle);
            indexInfo_ = *(IndexInfo *)p;
        }

        Index::~Index() {
        }

        void Index::commitIndexInfo() {
            char *p = handle_.getFileInfo();
            p += sizeof(rm::FileInfo);
            *(IndexInfo *)p = indexInfo_;
        }

        RID Index::getRootRID() {
            return RID(indexInfo_.rootPageNum, indexInfo_.rootSlotNum);
        }

        void Index::setRoot(RID rid) {
            indexInfo_.rootPageNum = rid.pageNum;
            indexInfo_.rootSlotNum = rid.slotNum;
            commitIndexInfo();
        }

        void Index::getNode(RID rid, Node &node) {
            char *pData = handle_.getRecDataPtr(rid) + FLAG_SIZE; // move flag
            memcpy(&node, pData, sizeof(node));
            handle_.releaseRecDataPtr(rid);
        }

        void Index::commitNode(RID rid, const Node &node) {
            char *pData = handle_.getRecDataPtr(rid) + FLAG_SIZE;
            memcpy(pData, &node, sizeof(node));
            handle_.commitPage(rid.pageNum);
            handle_.releaseRecDataPtr(rid);
        }

        RID Index::allocateNode() {
            RID r = handle_.declareRec();
            return r;
        }

        RID Index::releaseNode(RID rid) {
            handle_.deleteRec(rid);
            return rid;
        }

        void Index::forcePages() {
            handle_.forcePages();
        }

        int Index::getIndexSize() {
            return indexInfo_.indexSize;
        }

        void Index::incIndexSize() {
            indexInfo_.indexSize++;
        }

        void Index::decIndexSize() {
            indexInfo_.indexSize--;
        }

        RID Index::searchEntry(int key) {
            hot_ = getRootRID();
            Node v;
            getNode(hot_, v);
            while(true){
                int r = v.getPosition(key);
                if(!v.isLeaf){
                    hot_ = v.n[r+1];
                    getNode(hot_, v);
                }
                else{
                    if(r >= 0 && v.k[r] == key){
                        return v.n[r];
                    }
                    else{
                        return RID(-1, r+1); // refer to not find and the second value means the expected position
                    }
                }
            }
        }

        RID Index::insertEntry(int key, RID value) {
            RID v = searchEntry(key);
            if(v.pageNum >= 0)
                return v;
            int posi = v.slotNum;
            Node leaf;
            getNode(hot_, leaf);
            leaf.insertK(key, posi);
            leaf.insertN(value, posi);
            commitNode(hot_, leaf);
            incIndexSize();
            solveOverFlow(hot_);
            forcePages();
            return value;
        }

        bool Index::removeEntry(int key) {
            RID v = searchEntry(key);
            if(v.pageNum < 0)
                return false;
            Node leaf;
            getNode(hot_, leaf);
            int rank = leaf.getPosition(key);
            leaf.removeK(rank);
            leaf.removeN(rank);
            commitNode(hot_, leaf);
            decIndexSize();
            solveUnderFlow(hot_);
            forcePages();
            return true;
        }

        void Index::solveOverFlow(RID rid) {
            Node v;
            getNode(rid, v);
            if(v.isLeaf) {
                if (B_PLUS_TREE_BRANCH + 1 >= v.size) return;
                int half = (B_PLUS_TREE_BRANCH + 1) / 2;
                RID nid = allocateNode();
                Node n;
                getNode(nid, n);
                n.isLeaf = true;
                for (int j = 0; j < B_PLUS_TREE_BRANCH - half; j++) {
                    n.insertN(v.n[half + 1], j);
                    v.removeN(half + 1);
                    n.insertK(v.k[half + 1], j);
                    v.removeK(half + 1);
                }
                if(getRootRID() == rid){
                    RID rt = allocateNode();
                    setRoot(rt);
                    Node root;
                    getNode(rt, root);
                    root.isLeaf = false;
                    root.insertN(rid, 0);
                    v.parent = rt;
                }
                RID pid = v.parent;
                Node p;
                getNode(pid, p);
                int rank = p.getPosition(v.k[0]) + 1;
                p.insertK(v.k[half], rank);
                p.insertN(nid, rank + 1);
                n.parent = pid;
                commitNode(rid, v);
                commitNode(nid, n);
                commitNode(pid, p);
                solveOverFlow(pid);
            }
            else
            {
                if (B_PLUS_TREE_BRANCH + 1 >= v.size + 1) return;
                int half = (B_PLUS_TREE_BRANCH + 1) / 2;
                RID nid = allocateNode();
                Node n;
                getNode(nid, n);
                n.isLeaf = false;
                for (int j = 0; j < B_PLUS_TREE_BRANCH - half ; j++) {
                    n.insertN(v.n[half + 1], j);
                    v.removeN(half + 1);
                    n.insertK(v.k[half + 1], j);
                    v.removeK(half + 1);
                }
                n.insertN(v.n[half + 1], B_PLUS_TREE_BRANCH - half);
                v.removeN(half + 1);
                if(getRootRID() == rid){
                    RID rt = allocateNode();
                    setRoot(rt);
                    Node root;
                    getNode(rt, root);
                    root.isLeaf = false;
                    root.insertN(rid, 0);
                    v.parent = rt;
                }
                RID pid = v.parent;
                Node p;
                getNode(pid, p);
                int rank = p.getPosition(v.k[0]) + 1;
                p.insertK(v.k[half], rank);
                p.insertN(nid, rank + 1);
                v.removeK(half);
                n.parent = pid;
                commitNode(rid, v);
                commitNode(nid, n);
                commitNode(pid, p);
                solveOverFlow(pid);
            }
        }

        void Index::solveUnderFlow(RID rid) {
            Node v;
            getNode(rid, v);
            if(v.isLeaf){
                if((B_PLUS_TREE_BRANCH + 2) / 2 <= v.size) return;
                if(getRootRID() == rid) {
                    return;
                }
                RID pid = v.parent;
                Node p;
                getNode(pid, p);
                int r = 0;
                while(!(p.n[r] == rid)) r++;
                if(r > 0){
                    Node ls;
                    getNode(p.n[r-1], ls);
                    if((B_PLUS_TREE_BRANCH + 2) / 2 < ls.size){
                        p.k[r-1] = ls.k[ls.size - 1];
                        v.insertN(ls.n[ls.size - 1], 0);
                        ls.removeN(ls.size - 1);
                        v.insertK(ls.k[ls.size - 1], 0);
                        ls.removeK(ls.size - 1);
                        commitNode(p.n[r-1], ls);
                        commitNode(rid, v);
                        commitNode(pid, p);
                        return;
                    }
                }
                if(r < p.size){
                    Node rs;
                    getNode(p.n[r+1], rs);
                    if((B_PLUS_TREE_BRANCH + 2) / 2 < rs.size){
                        v.insertN(rs.n[0], v.size);
                        rs.removeN(0);
                        v.insertK(rs.k[0], v.size);
                        rs.removeK(0);
                        p.k[r] = rs.k[0];
                        commitNode(p.n[r+1], rs);
                        commitNode(rid, v);
                        commitNode(pid, p);
                        return;
                    }
                }
                if(r > 0){
                    Node ls;
                    getNode(p.n[r-1], ls);
                    int len = ls.size;
                    for(int j = len - 1 ; j >= 0 ; j --)
                    {
                        v.insertN(ls.n[j], 0);
                        ls.removeN(j);
                        v.insertK(ls.k[j], 0);
                        ls.removeK(j);
                    }
                    releaseNode(p.n[r-1]);
                    p.removeN(r-1);
                    p.removeK(r-1);
                    commitNode(rid, v);
                    commitNode(pid, p);
                }
                else
                {
                    Node rs;
                    getNode(p.n[r+1], rs);
                    int len = rs.size;
                    int nlen = v.size;
                    for(int j = len - 1 ; j >= 0 ; j --)
                    {
                        v.insertN(rs.n[j], nlen);
                        rs.removeN(j);
                        v.insertK(rs.k[j], nlen);
                        rs.removeK(j);
                    }
                    releaseNode(p.n[r+1]);
                    p.removeN(r+1);
                    p.removeK(r);
                    commitNode(rid, v);
                    commitNode(pid, p);
                }
                solveUnderFlow(pid);
                return;
            }
            else{
                if((B_PLUS_TREE_BRANCH + 2) / 2 <= v.size) return;
                if(getRootRID() == rid) {
                    return;
                }
                RID pid = v.parent;
                Node p;
                getNode(pid, p);
                int r = 0;
                while(!(p.n[r] == rid)) r++;
                if(r > 0){
                    Node ls;
                    getNode(p.n[r-1], ls);
                    if((B_PLUS_TREE_BRANCH + 2) / 2 < ls.size){
                        v.insertN(ls.n[ls.size], 0);
                        ls.removeN(ls.size);
                        v.insertK(p.k[r-1], 0);
                        p.k[r-1] = ls.k[ls.size - 1];
                        ls.removeK(ls.size - 1);
                        commitNode(p.n[r-1], ls);
                        commitNode(rid, v);
                        commitNode(pid, p);
                        return;
                    }
                }
                if(r < p.size){
                    Node rs;
                    getNode(p.n[r+1], rs);
                    if((B_PLUS_TREE_BRANCH + 2) / 2 < rs.size){
                        v.insertN(rs.n[0], v.size + 1);
                        rs.removeN(0);
                        v.insertK(p.k[r], v.size);
                        p.k[r] = rs.k[0];
                        rs.removeK(0);
                        commitNode(p.n[r+1], rs);
                        commitNode(rid, v);
                        commitNode(pid, p);
                        return;
                    }
                }
                if(r > 0){
                    Node ls;
                    getNode(p.n[r-1], ls);
                    int len = ls.size;
                    v.insertK(p.k[r-1], 0);
                    v.insertN(ls.n[len], 0);
                    v.removeN(len);
                    for(int j = len - 1 ; j >= 0 ; j --)
                    {
                        v.insertN(ls.n[j], 0);
                        ls.removeN(j);
                        v.insertK(ls.k[j], 0);
                        ls.removeK(j);
                    }
                    releaseNode(p.n[r-1]);
                    p.removeN(r-1);
                    p.removeK(r-1);
                    commitNode(rid, v);
                    commitNode(pid, p);
                }
                else
                {
                    Node rs;
                    getNode(p.n[r+1], rs);
                    int len = rs.size;
                    int nlen = v.size;
                    v.insertK(p.k[r], nlen);
                    for(int j = 0 ; j < len ; j ++)
                    {
                        v.insertN(rs.n[0], nlen + 1 + j);
                        rs.removeN(0);
                        v.insertK(rs.k[0], nlen + 1 + j);
                        rs.removeK(0);
                    }
                    v.insertN(rs.n[0], nlen + 1 + len);
                    rs.removeN(0);
                    releaseNode(p.n[r+1]);
                    p.removeN(r+1);
                    p.removeK(r);
                    commitNode(rid, v);
                    commitNode(pid, p);
                }
                solveUnderFlow(pid);
                return;
            }
        }

        void Index::getRoot(Node &node) {
            getNode(getRootRID(), node);
        }

    }
}