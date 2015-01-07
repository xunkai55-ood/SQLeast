#include "rm/core.h"
#include "rm/filehandle.h"
#include "rm/exception.h"
#include "rm/bitmaputil.h"

namespace sqleast {
    namespace rm {

        using namespace pagefs;

        void RecordManager::createFile(const char *fileName, int recordSize, bool override) {
            PageFS &fs = PageFS::getInstance();
            fs.createFile(fileName, override);
            int fid = fs.openFile(fileName);
            rm::FileHandle handle(fid);

            FileInfo *infoPtr = (FileInfo*) fs.loadPage(fid, 0);
            infoPtr->firstEmptyPage = 0;
            infoPtr->recordSize = recordSize;
            // slot_n = 8 * (PAGE_SIZE - HEADER_SIZE) / (8 * rsize + 1)
            infoPtr->slotPerPage = ((PAGE_SIZE - (int)sizeof(PageHeader) + (int)sizeof(char *)) << 3) / ((recordSize << 3) + 1);
            infoPtr->slotBitmapSize = infoPtr->slotPerPage / 8 + (infoPtr->slotPerPage % 8 > 0);
            infoPtr->totalPageNum = 1;

            if (infoPtr->slotPerPage <= 0)
                throw RecordTooLargeException();

            std::cout << "..." << std::endl;
            fs.markDirty(fid, 0);
            fs.forcePage(fid, 0);
            fs.unpinPage(fid, 0);
            std::cout << "!!!" << std::endl;
        }

        FileHandle RecordManager::openFile(const char *fileName) {
            PageFS &fs = PageFS::getInstance();
            int fid = fs.openFile(fileName);
            return rm::FileHandle(fid);
        }

        void RecordManager::destroyFile(const char *fileName) {
            PageFS &fs = PageFS::getInstance();
            fs.destroyFile(fileName);
        }

    }
}