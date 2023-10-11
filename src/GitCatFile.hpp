#pragma once

#include "precompile.h"

#include <string>

#include "common.hpp"
#include "sha1_proxy.hpp"
#include "objects_proxy.hpp"

std::string catFile(SHAString hash);

inline ObjectType catFileType(SHAString hash) {
    return GitObjectsProxy::getInstance().getObjectType(hash);
}