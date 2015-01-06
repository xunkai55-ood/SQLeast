#include "pagefs/pagefs.h"
#include <iostream>
#include <cstring>
#include <cstdio>

namespace pagefs {

    /* pagefs */

    PageFS::PageFS(): entryCnt_(0) {
        lruList_ = LRUList();
        lruTable_ = LRUHash();
        for (int i = 0; i < MAX_BUFFER_SIZE; i++) {
            lruTable_.table[i].node = nullptr;
        }
    }

    PageFS::~PageFS() {
        std::cout << "destructing pagefs" << std::endl;
        commitAll(-1);
    }

    void PageFS::printState(std::ostream &os) {
        os << "[PageFS state]" << std::endl;
        os << "[buffer] " << lruTable_.total << " / " << MAX_BUFFER_SIZE << std::endl;
    }
    // TODO check file names

    void PageFS::createFile(const char *fileName, bool override) {
        FILE *f = fopen(fileName, "r");
        if (f != nullptr) {
            fclose(f);
            if (!override) {
                throw FileExistsException();
            }
        }
        f = fopen(fileName, "w");
        if (f == nullptr) {
            throw FileNotFoundException();
        }
        fclose(f);
    }

    void PageFS::destroyFile(const char *fileName) {
        if (remove(fileName) != 0) {
            throw FileRemoveException();
        }
    }

    FileId PageFS::openFile(const char *fileName) {
        // TODO read-only file handles
        // TODO use TRIE, hash or any other high level tech to control the files.

        for (int i = 0; i < entryCnt_; i++)
            if (entries_[i].fileId >= 0 && strcmp(fileName, entries_[i].fileName) == 0) {
                // Hint: a file can only has a file handle alive
                throw FileOpenedException();
            }

        int k;
        for (k = 0; k < entryCnt_ && entries_[k].fileId >= 0; k++);  // find empty position
        if (k == entryCnt_) {
            if (entryCnt_ == MAX_FILE_NUM) {
                throw TooManyFilesError();
            }
            entryCnt_++;
        }

        FILE *f = fopen(fileName, "rb+");
        strcpy(entries_[k].fileName, fileName);
        entries_[k].fileId = entryCnt_;
        entries_[k].counter = 1;
        filePtr_[k] = f;

        return k;
    }

    void PageFS::closeFile(FileId f) {
        // TODO use TRIE, hash or any other high level tech to control the files.
        if (entries_[f].counter == 0) {
            throw FileClosedException();
        }
        commitAll(f);
        entries_[f].counter -= 1;
        if (entries_[f].counter == 0) {
            entries_[f].fileId = -1;  // release
            fclose(filePtr_[f]);
        }
    }

    char *PageFS::loadPage(int fileId, int pageNum) {
        LRUHashItem *t;
        try {
            t = lruTable_.get(fileId, pageNum);
        } catch (const ItemNotFound &e) {
            t = nullptr;
        }
        if (t == nullptr) {
            if (lruTable_.total == MAX_BUFFER_SIZE) {
                if (!commitOnePage())
                    throw NoBufError();
            }
//            Debug::info("not found item");
            BufferPage p;
            p.data = new char[PAGE_SIZE];
            memset(p.data, 0, PAGE_SIZE);
            p.fileId = fileId;
            p.pageNum = pageNum;
            p.dirty = 0;
            p.pinned = 1;  // automatically pinned

            FILE *f = filePtr_[fileId];
            fseek(f, 0, SEEK_END);
            off_t offset = PAGE_SIZE * pageNum;
            if (offset <= ftell(f)) {
                fseek(f, pageNum * PAGE_SIZE, 0);
                fread(p.data, 1, PAGE_SIZE, f);
            }

            t = lruTable_.add(p, nullptr);
            t->node = lruList_.push_back(t);

            return p.data;
        } else {
//            Debug::info("found item");
            lruList_.remove(t->node);
            t->node = lruList_.push_back(t);
            return t->data.data;
        }
    }

    void PageFS::forcePage(int fileId, int pageNum) {
        LRUHashItem *t = lruTable_.get(fileId, pageNum);
        writeBack(t->data);
        t->data.dirty = 0;
        lruList_.remove(t->node);
        t->node = lruList_.push_back(t);
    }

    void PageFS::pinPage(int fileId, int pageNum) {
        LRUHashItem *p = lruTable_.get(fileId, pageNum);
        p->data.pinned = 1;
    }

    void PageFS::unpinPage(int fileId, int pageNum) {
        LRUHashItem *p = lruTable_.get(fileId, pageNum);
        p->data.pinned = 0;
    }

    void PageFS::markDirty(int fileId, int pageNum) {
        LRUHashItem *p = lruTable_.get(fileId, pageNum);
        p->data.dirty = 1;
    }

    bool PageFS::commitOnePage() {
        LRUListNode *p = lruList_.head;
        while (p != nullptr && p->item->data.pinned)
            p = p->next;
        Debug::info("found unpined");
        if (p == nullptr)
            return false;
        LRUHashItem *t = lruList_.remove(p);
        int key = (int)(t - lruTable_.table);
        return writeBack(lruTable_.popByKey(key).data);
    }

    void PageFS::commitAll(int fid) {
        LRUListNode *p = lruList_.head, *q;
        while (p != nullptr) {
            q = p->next;
            if (fid < 0 || p->item->data.fileId == fid) {
                LRUHashItem *t = lruList_.remove(p);
                int key = (int)(t - lruTable_.table);
                writeBack(lruTable_.popByKey(key).data);
            }
            p = q;
        }
    }

    bool PageFS::writeBack(BufferPage p) {
        if (p.dirty) {
            FILE *f = filePtr_[p.fileId];
            fseek(f, p.pageNum * PAGE_SIZE, 0);
            fwrite(p.data, 1, PAGE_SIZE, f);
        }
        return true;
    }


    /* LRU hash */

    LRUHash::LRUHash() : total(0) {
        for (int i = 0; i < MAX_BUFFER_SIZE; i++) {
            table[i].node = nullptr;
        }
    }

    LRUHash::~LRUHash() {

    }

    LRUHashItem *LRUHash::add(BufferPage &p, LRUListNode *node) {
        if (total == MAX_BUFFER_SIZE) {
            throw HashTableError();  // PageFS should be responsible for clearing data
        }
        total += 1;
        int key = hash(p.fileId, p.pageNum);
        int hashValue = key;
        while (table[key].node != nullptr) {
            key = (key + 1) & MAX_BUFFER_SIZE_M1;
        }
        table[key].data = p;
        table[key].hashValue = hashValue;
        table[key].node = node;
        return &(table[key]);
    }

    LRUHashItem *LRUHash::get(int fileId, int pageNum) {
        int key = hash(fileId, pageNum);
        int hashValue = key;
        if (table[key].node == nullptr)
            return nullptr;
        while (table[key].data.fileId != fileId || table[key].data.pageNum != pageNum) {
            key = (key + 1) & MAX_BUFFER_SIZE_M1;
            if (table[key].node == nullptr)
                return nullptr;
            if (key == hashValue) {
                throw ItemNotFound();
            }
        }
        return &(table[key]);
    }

    LRUHashItem LRUHash::pop(int fileId, int pageNum) {
        int key = hash(fileId, pageNum);
        int hashValue = key;
        if (table[key].node == nullptr)
            throw ItemNotFound();
        while (table[key].data.fileId != fileId || table[key].data.pageNum != pageNum) {
            key = (key + 1) & MAX_BUFFER_SIZE_M1;
            if (table[key].node == nullptr || key == hashValue) {
                throw ItemNotFound();
            }
        }
        return popByKey(key);
    }

    LRUHashItem LRUHash::popByKey(int j) {
        /* see http://en.wikipedia.org/wiki/Open_addressing */
        int i = j, k;
        total -= 1;
        LRUHashItem res = table[i];
        Debug::info("start popping");
        std::cout << i << std::endl;
        table[i].node = nullptr;
        do {
            j = (j + 1) & MAX_BUFFER_SIZE_M1;
            if (table[j].node == nullptr)
                break;
            k = table[j].hashValue;
            if ((i <= j) ? ((i < k) && (k <= j)) : (i < k) || (k <= j))
                continue;
            table[i] = table[j];
            table[j].node = nullptr;
            i = j;
        } while (true);
        Debug::info("popped");
        return res;
    }

    /* LRU list */
    LRUList::LRUList() {
        head = nullptr;
        tail = nullptr;
    }

    LRUList::~LRUList() {
        // TODO release
    }

    LRUListNode *LRUList::push_head(LRUHashItem *p) {
        LRUListNode *n = new LRUListNode;
        n->item = p;
        n->prev = nullptr;
        n->next = head;
        if (head != nullptr) head->prev = n;
        head = n;
        if (tail == nullptr) tail = head;
        return head;
    }

    LRUListNode *LRUList::push_back(LRUHashItem *p) {
        LRUListNode *n = new LRUListNode;
        n->item = p;
        n->prev = tail;
        n->next = nullptr;
        if (tail != nullptr) tail->next = n;
        tail = n;
        if (head == nullptr) head = tail;
        return tail;
    }

    LRUHashItem *LRUList::remove(LRUListNode *p) {
        if (p == nullptr) return nullptr;
        if (p == head) head = p->next;
        if (p == tail) tail = p->prev;
        LRUHashItem *res = p->item;
        LRUListNode *q = p->prev, *r = p->next;
        if (q != nullptr) q->next = r;
        if (r != nullptr) r->prev = q;
        delete p;
        return res;
    }
}
