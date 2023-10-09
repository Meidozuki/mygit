
#pragma once

#include "sha1_proxy.hpp"
#include "FP_util.hpp"

SHAString commitTree(std::string message, SHAString tree, Option<SHAString> parent = std::nullopt);
