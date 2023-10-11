#include "precompile.h"

#include "GitCatFile.hpp"

std::string catFile(SHAString hash) {
    if (auto str = GitObjectsProxy::getInstance().readObject(hash)) {
        return str.value().str();
    }
    else {
        throw std::invalid_argument("hash does not exist.");
    }
}