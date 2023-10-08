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


#include <cstdio>
static bool testSSO(bool verbose=true) {
    bool flag=true;
    for (auto example: {"small", "", "1", "123", "12345", "blob 12", "commit 123"}) {
        std::string s(example);
        if (verbose)
            printf("size = %ld, capacity = %ld, string=%s\n", s.size(), s.capacity(), example);
        if (s.size() == s.capacity())
            flag = false;
    }
    return flag;
}