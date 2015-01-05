#ifndef RM_EXCEPTION_H
#define RM_EXCEPTION_H

namespace sqleast {
    namespace rm {

        class RMException {};

        class RecordTooLargeException : public RMException {};
    }
}

#endif