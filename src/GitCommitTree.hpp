
#pragma once

#include "sha1_proxy.hpp"
#include "FP_util.hpp"

// TODO: ShaString is supposed to be trivially-copyable, check the function arguments
SHAString commitTree(SHAString tree, Option<SHAString> parent=std::nullopt, std::string message="");
