#ifndef SM_EXCEPTION_H
#define SM_EXCEPTION_H

namespace sqleast {
    namespace sm {

        class SMException {};

        class DBExistsException: public SMException {};

        class DBNotExistsException: public SMException {};
    }
}

#endif