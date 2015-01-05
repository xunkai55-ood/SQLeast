#ifndef RM_CORE_H
#define RM_CORE_H

#include "pagefs/pagefs.h"

namespace sqleast {
    namespace rm {

        struct FileInfo {
            int recordSize;
            int slotPerPage;
            int slotBitmapSize;
            int firstEmptyPage;
            int totalPageNum;
        };

        struct PageHeader {
            int nextPage;
            int emptySlot;
            char *slotBitmap;

            PageHeader (int _nextPage, int _emptySlot, char *_slotBitmap):
                    nextPage(_nextPage), emptySlot(_emptySlot), slotBitmap(_slotBitmap) {}
        }; // should be created and released by core, cuz slotBitmap is variable-length

        class FileHandle;

        class RecordManager {

            static void createFile(const char *fileName, int dataSize);
            static FileHandle openFile(const char *fileName);
            static void destroyFile(const char *fileName);

        };
    }
}

#endif