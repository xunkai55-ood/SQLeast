#include "rm/bitmaputil.h"
#include "rm/filehandle.h"
#include "rm/exception.h"

namespace sqleast {
    namespace rm {

        using namespace pagefs;

        FileHandle::FileHandle(FileId fid) : fid_(fid), fs_(PageFS::getInstance()) {
            FileInfo *infoPtr = (FileInfo*)fs_.loadPage(fid, 0);
            info_ = *(infoPtr);
        }

        FileHandle::~FileHandle() {
            forcePages();
        }

        void FileHandle::getRec(RID rid, Record &r) {
            if (r.size != info_.recordSize)
                throw RecordSizeError();
            char *pData = fs_.loadPage(fid_, rid.pageNum);
            pData = moveToRec(pData);
            pData += rid.slotNum * info_.recordSize;
            memcpy(r.rData, pData, (size_t)r.size);
            r.rid = rid;
        }

        void FileHandle::updateRec(const Record &r) {
            if (r.rid.pageNum <= 0 || r.rid.slotNum < 0)
                throw InvalidRecordException();
            if (r.size != info_.recordSize)
                throw RecordSizeError();
            char *pData = fs_.loadPage(fid_, r.rid.pageNum);
            pData = moveToRec(pData);
            pData += r.rid.slotNum * info_.recordSize;
            memcpy(pData, r.rData, (size_t)(info_.recordSize));
            commitPage(r.rid.pageNum);
        }

        RID FileHandle::allocateRec() {
            int pageNum = info_.firstEmptyPage;
            int slotNum = 0;
            while (true) {
                if (pageNum == 0) pageNum = newPage();
                while (pageNum != 0) {
                    char *pData = fs_.loadPage(fid_, pageNum);
                    PageHeader pHeader = getPageHeader(pData);
                    char *bitmap = pHeader.slotBitmap;
                    char *records = pHeader.slotBitmap + info_.slotBitmapSize;
                    bool found = false;

                    while (bitmap < records) {
                        if ((unsigned char) (*bitmap) != 255) { // find avaliable slot
                            slotNum = Bitmap8Util::lowest0((unsigned char) *bitmap);
                            *bitmap |= 1 << slotNum;
                            pHeader.emptySlot -= 1;
                            if (pHeader.emptySlot == 0) { // remove the page from empty array
                                if (pHeader.nextPage != 0) {
                                    char *next = fs_.loadPage(fid_, pHeader.nextPage);
                                    PageHeader nextPH = getPageHeader(next);
                                    writePageHeader(next, nextPH);
                                }
                                if (pageNum == info_.firstEmptyPage) {
                                    info_.firstEmptyPage = pHeader.nextPage;
                                    commitInfo();
                                }
                            }
                            writePageHeader(pData, pHeader);
                            found = true;
                            slotNum += (int) ((bitmap - pHeader.slotBitmap) << 3);
                            break;
                        }
                        bitmap += 1;
                    }
                    if (!found) {
                        pageNum = pHeader.nextPage;
                    } else {
                        break;
                    }
                }
                if (pageNum > 0) break;
            }
            return RID(pageNum, slotNum);
        }

        void FileHandle::insertRec(Record &r) {
            r.rid = allocateRec();
            updateRec(r);
        }

        void FileHandle::deleteRec(const Record &r) {
            RID rid = r.rid;
            char *pData = fs_.loadPage(fid_, rid.pageNum);
            PageHeader pHeader = getPageHeader(pData);
            if (pHeader.emptySlot == 0) {
                pHeader.nextPage = info_.firstEmptyPage;
                info_.firstEmptyPage = rid.pageNum;
                commitInfo();
            }
            pHeader.emptySlot += 1;
            *(pHeader.slotBitmap + (rid.slotNum >> 3)) &= ~(1 << (rid.slotNum & 7));
            writePageHeader(pData, pHeader);
            pData = moveToRec(pData);
            *(unsigned int *)pData &= ~REC_ALIVE;
            commitPage(rid.pageNum);
        }

        int FileHandle::newPage() {
            char *pData = fs_.loadPage(fid_, info_.totalPageNum);

            PageHeader pHeader = getPageHeader(pData);
            pHeader.emptySlot = info_.slotPerPage;
            pHeader.nextPage = info_.firstEmptyPage;
            info_.firstEmptyPage = info_.totalPageNum;
            writePageHeader(pData, pHeader);

            char *bitmap = pHeader.slotBitmap;
            memset(bitmap, 0, (size_t)info_.slotBitmapSize);
            if (info_.slotPerPage % 8) {
                bitmap += info_.slotBitmapSize - 1;
                *bitmap = (unsigned char)(255) << (info_.slotPerPage % 8);
            }
            commitPage(info_.totalPageNum);
            info_.totalPageNum += 1;
            commitInfo();
            return info_.totalPageNum - 1;
        }

        void FileHandle::forcePages() {
            fs_.forcePage(fid_, pagefs::ALL_PAGES);
        }

    }
}

