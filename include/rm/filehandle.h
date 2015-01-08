#ifndef RM_FILEHANDLE_H
#define RM_FILEHANDLE_H

#include "recordmanager.h"

namespace sqleast {
    namespace rm {

        class FileHandle {

        public:
            FileHandle(FileId fid);
            ~FileHandle();

            void getRec(RID rid, Record &r);
            void insertRec(Record &r);
            RID declareRec();
            void updateRec(Record &r);
            void deleteRec(const Record &r);
            void deleteRec(RID rid);
            void forcePages();
            inline FileInfo getInfo() { return info_; }
            inline char *getFileInfo() { return fs_.loadPage(fid_, 0); }

            /* for performance concern, 3 dangerous quick callable */
            char *getRecDataPtr(RID rid);
            inline void releaseRecDataPtr(RID rid) { unpinPage(rid.pageNum); }
            inline void commitPage(int pageNum) { fs_.markDirty(fid_, pageNum); }

        private:
            FileId fid_;
            FileInfo info_;
            pagefs::PageFS &fs_;

            inline void commitInfo() {
                char *pData = fs_.loadPage(fid_, 0);
                memcpy(pData, &info_, sizeof(info_));
                fs_.markDirty(fid_, 0);
            }

            inline void unpinPage(int pageNum) {
                fs_.unpinPage(fid_, pageNum);
            }

            inline PageHeader getPageHeader(char *pData) {
                return PageHeader(
                        *(int*)(pData),  // next page
                        *(int*)(pData + sizeof(int)),  // empty slot num
                        pData + sizeof(PageHeader) - sizeof(char *)  // bitmap
                );
            }

            inline void writePageHeader(char *pData, PageHeader &pHeader) {
                *(int*)pData = pHeader.nextPage;
                pData += sizeof(int);
                *(int*)pData = pHeader.emptySlot;
                pData += sizeof(int);
            }

            inline char *moveToRec(char *pData) {
                PageHeader header = getPageHeader(pData);
                return header.slotBitmap + info_.slotBitmapSize;
            }

            int newPage();
            RID allocateRec();
        };
    }
}

#endif