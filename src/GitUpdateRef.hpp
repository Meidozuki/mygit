#pragma once

#include "precompile.h"

#include <string>
#include <fstream>
#include <stdexcept>

#include "common.hpp"
#include "sha1_proxy.hpp"
#include "objects_proxy.hpp"
#include "GitCatFile.hpp"

inline
void updateRef(const Path& ref_name, SHAString new_value) {
    using namespace std::string_literals;
    if (filesys::equivalent(ref_name.parent_path(), GitObjectsProxy::getGitDir())
     || filesys::equivalent(ref_name.parent_path(), GitObjectsProxy::getRefsDir())) {
        std::ofstream fs(ref_name, std::ios_base::out); // overwrite if exists

        conditionCheck(fs.is_open(), std::runtime_error("update-ref: failed to open given ref name "s + ref_name.string()));
        conditionCheck(catFileType(new_value) == ObjectType::kCommit,
                       std::invalid_argument("update-ref: object "s + std::string(new_value) + " is not a commir"));

        fs << new_value;
    }
}