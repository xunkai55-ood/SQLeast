#ifndef PAGEFS_H
#define PAGEFS_H

#include <cstdlib>
#include <cstdio>

#include "exception.h"

#include "../debug.h"

namespace pagefs {

    // pagefs
    const int MAX_FILE_NUM(32767);
    const int PAGE_SIZE(256);
    const int MAX_BUFFER_SIZE(4); // has to be 2^n for the performance concern
    const int MAX_BUFFER_SIZE_M1 = MAX_BUFFER_SIZE - 1;

    // file system
    const int FILE_NAME_MAX_LEN(40);

    typedef int FileId;

    struct FileEntry {
        char fileName[FILE_NAME_MAX_LEN];
        int fileId;
        int counter;
    };

    struct BufferPage {
        char *data;
        int dirty;
        int pinned;
        int fileId;
        int pageNum;
    };

    struct LRUListNode;
    struct LRUHashItem;

    struct LRUListNode {
        LRUListNode *next, *prev;
        LRUHashItem *item;
    };

    struct LRUHashItem {
        BufferPage data;
        LRUListNode *node;  // nullptr means unused
        int hashValue;
    };

    struct LRUHash {

        LRUHash();
        ~LRUHash();
        LRUHashItem table[MAX_BUFFER_SIZE];
        int total;

        inline int hash(int fileId, int pageNum) {
            return ((fileId << 5) + pageNum) & MAX_BUFFER_SIZE_M1;
        }

        LRUHashItem *get(int fileId, int pageNum);
        LRUHashItem pop(int fileId, int pageNum);
        LRUHashItem popByKey(int key);
        LRUHashItem *add(BufferPage &p, LRUListNode *node);

    };

    struct LRUList {

        LRUList();
        ~LRUList();

        LRUListNode *push_head(LRUHashItem *p);
        LRUListNode *push_back(LRUHashItem *p);
        inline LRUHashItem *pop_head() { return remove(head); }
        inline LRUHashItem *pop_back() { return remove(tail); }
        LRUHashItem *remove(LRUListNode *p);

        LRUListNode *head, *tail;
    };

    class PageFS {

    public:

        static PageFS *getInstance() {
            if (instance_ == nullptr) {
                instance_ = new PageFS();
            }
            return instance_;
        }

        ~PageFS();

        void createFile(const char *fileName, bool override = false);
        void destroyFile(const char *fileName);
        FileId openFile(const char *fileName);
        void closeFile(FileId f);
        char *loadPage(int fileId, int pageNum);
        void forcePage(int fileId, int pageNum);
        void pinPage(int fileId, int pageNum);
        void unpinPage(int fileId, int pageNum);
        void markDirty(int fileId, int pageNum);


    private:
        static PageFS *instance_;

        PageFS();

        FileEntry entries_[MAX_FILE_NUM];
        int entryCnt_;
        FILE* filePtr_[MAX_FILE_NUM];
        LRUHash lruTable_;
        LRUList lruList_;

        /* buffer */

        bool commitOnePage();
        bool writeBack(BufferPage p);

    };

};

#endif