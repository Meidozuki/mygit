//
// Created by user on 2023/9/3.
//

#pragma once

#include <string>
#include <string_view>

#include "common.hpp"

using StringType = std::string;
using SV = std::string_view;

StringType hashObject(SV str);
StringType hashObjectInterface(SV msg,
                               InArgType arg_type = InArgType::kRawString,
                               ObjectType type = ObjectType::kBlob,
                               bool if_write = false);
