//
// 对应命令 git hash-object
// 由于需要cryptopp库，强烈建议单独成一个文件
//

#pragma once

#include "precompile.h"

#include <string_view>

#include "common.hpp"
#include "sha1_proxy.hpp"

using StringView = std::string_view;

SHAString hashObject(StringView str);
SHAString hashObjectInterface(StringView msg,
                               InArgType arg_type = InArgType::kRawString,
                               ObjectType type = ObjectType::kBlob,
                               bool if_write = false);

// Test SSO which will be used to create header
bool testSSO(bool verbose=true);