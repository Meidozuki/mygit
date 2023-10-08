//
// Created by user on 2023/9/3.
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
static bool testSSO() {
    bool flag=true;
    for (auto example: {"small", "", "1", "123", "12345", "blob 12", "commit 123"}) {
        std::string s(example);
        printf("size = %ld, capacity = %ld, string=%s\n", s.size(), s.capacity(), example);
        if (s.size() == s.capacity()) flag = false;
    }
    return flag;
}