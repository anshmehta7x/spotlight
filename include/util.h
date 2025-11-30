//
// Created by a7x on 01/12/2025.
//

#ifndef SPOTLIGHT_UTIL_H
#define SPOTLIGHT_UTIL_H

#include <unistd.h>

inline bool check_sudo() {
    if (geteuid() == 0) {
        return true;
    }
    return false;
}

#endif //SPOTLIGHT_UTIL_H