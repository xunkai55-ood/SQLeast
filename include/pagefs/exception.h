#ifndef PAGEFS_EXCEPTION_H
#define PAGEFS_EXCEPTION_H

namespace pagefs {

    /* expected exceptions
     *
     * These exceptions may occur in runtime. They should be derived from
     *   PageFSException.
     */

    class PageFSException {};

    class EOFException : public PageFSException {};

    // page is already pinned
    class PagePinnedException : public PageFSException {};

    // page is not in buf
    class PageNotInBufException : public PageFSException {};

    // page is unpinned
    class PageUnpinnedException : public PageFSException {};

    // page is already freed
    class PageFreedException : public PageFSException {};

    // invalid page number
    class InvalidPageException : public PageFSException {};

    // file is opened
    class FileOpenedException : public PageFSException {};

    // file is closed
    class FileClosedException : public PageFSException {};

    // file existed
    class FileExistsException : public PageFSException {};

    // file not found
    class FileNotFoundException : public PageFSException {};

    // file table full
    class FileTableFullException : public PageFSException {};

    // exception when removing a file
    class FileRemoveException : public PageFSException {};

    /* unexpected exceptions
     *
     * These exceptions are not expected to occur in runtime.
     */

    class PageFSError {};

    class NoMemError : public PageFSError {};

    class NoBufError : public PageFSError {};

    class TooManyFilesError : public PageFSError {};

    // errors when reading files
    class ReadFileError : public PageFSError {};

    // errors when writing files
    class WriteFileError : public PageFSError {};

    // duplicated buffer pages
    class PageInBufError : public PageFSError {};

    class HashTableError : public PageFSError {};

}

#endif