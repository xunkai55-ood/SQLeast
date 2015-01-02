#ifndef SQLEAST_DEBUG_H
#define SQLEAST_DEBUG_H

#include <iostream>

class Debug {
public:
    static void info(const char *msg) {
        std::cout << "[INFO]" << msg << std::endl;
    }

};

#endif