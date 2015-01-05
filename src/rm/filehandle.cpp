#include "rm/bitmaputil.h"
#include "rm/filehandle.h"

namespace sqleast {
    namespace rm {

        using namespace pagefs;

        FileHandle::FileHandle(FileId fid) : fid_(fid) {
            fs_ = PageFS::getInstance();
            FileInfo *infoPtr = (FileInfo*)fs_->loadPage(fid, 0);
            info_ = *(infoPtr);
        }

        FileHandle::~FileHandle() {

        }

        Record FileHandle::getRec(const RID rid) {
            char *pData = fs_->loadPage(fid_, rid.pageNum);
            pData = moveToRec(pData);
            pData += rid.slotNum * info_.recordSize;
            return Record(rid, pData);
        }

        RID FileHandle::updateRec(const RID rid, char *const rData) {
            char *pData = fs_->loadPage(fid_, rid.pageNum);
            pData = moveToRec(pData);
            pData += rid.slotNum * info_.recordSize;
            memcpy(rData, pData, (size_t)info_.recordSize);
            commitPage(rid.pageNum);
            return RID(rid.pageNum, rid.slotNum);
        }

        RID FileHandle::insertRec(char *const rData) {
            int pageNum = info_.firstEmptyPage;
            int slotNum = 0;
            while (pageNum != 0) {
                char *pData = fs_->loadPage(fid_, pageNum);
                PageHeader pHeader = getPageHeader(pData);
                char *bitmap = pHeader.slotBitmap;
                char *records = pHeader.slotBitmap + info_.slotBitmapSize;
                bool found = false;

                while (bitmap < records) {
                    if ((unsigned char)(*bitmap) != 255) { // find avaliable slot
                        slotNum = Bitmap8Util::lowest0((unsigned char)*bitmap);
                        *bitmap |= 1 << slotNum;
                        pHeader.emptySlot -= 1;
                        if (pHeader.emptySlot == 0) { // remove the page from empty array
                            if (pHeader.nextPage != 0) {
                                char *next = fs_->loadPage(fid_, pHeader.nextPage);
                                PageHeader nextPH = getPageHeader(next);
                                writePageHeader(next, nextPH);
                            }
                            if (pageNum == info_.firstEmptyPage) {
                                info_.firstEmptyPage = pHeader.nextPage;
                                commitInfo();
                            }
                        }
                        commitInfo();
                        found = true;
                        slotNum += (int)((bitmap - pHeader.slotBitmap) << 3);
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
            updateRec(RID(pageNum, slotNum), rData);
            return RID(pageNum, slotNum);
        }

        void FileHandle::deleteRec(const RID rid) {
            char *pData = fs_->loadPage(fid_, rid.pageNum);
            PageHeader pHeader = getPageHeader(pData);
            if (pHeader.emptySlot == 0) {
                pHeader.nextPage = info_.firstEmptyPage;
                info_.firstEmptyPage = rid.pageNum;
                commitInfo();
            }
            pHeader.emptySlot += 1;
            *(pHeader.slotBitmap + (rid.slotNum >> 3)) &= ~(1 << (rid.slotNum & 7));
            writePageHeader(pData, pHeader);
            commitPage(rid.pageNum);
        }

        int FileHandle::newPage() {
            info_.totalPageNum += 1;
            char *pData = fs_->loadPage(fid_, info_.totalPageNum);

            PageHeader pHeader = getPageHeader(pData);
            pHeader.emptySlot = info_.slotPerPage;
            pHeader.nextPage = info_.firstEmptyPage;
            info_.firstEmptyPage = info_.totalPageNum;
            writePageHeader(pData, pHeader);
            commitPage(info_.totalPageNum);

            char *bitmap = pHeader.slotBitmap;
            memset(bitmap, 0, (size_t)info_.slotBitmapSize);
            commitInfo();
            return info_.totalPageNum;
        }

    }
}

