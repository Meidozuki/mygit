#include "precompile.h"

#include "GitCatFile.hpp"

std::string catFile(SHAString hash) {
    if (auto str = GitObjectsProxy::getInstance().catFile(hash)) {
        return str.value();
    }
    else {
        throw std::invalid_argument("hash does not exist.");
    }
}