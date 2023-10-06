
#pragma once

#include "precompile.h"

#include <filesystem>

#include "common.hpp"
#include "index.hpp"

void updateIndex(Index &index, const Path& filename, std::error_code &ec);

inline void updateIndex(Index &index, const Path& filename) {
    std::error_code ec;
    updateIndex(index, filename, ec);
    if ((bool) ec) {
        std::cerr << ec.message() << '\n';
    }
}

/**
 * @param mode
 * @param hash should a valid SHA1 that already adds to Objects Database. (原始git不检查是否存在)
 * @param path 可以为空路径，此时必须指定--add （对应的file size为0）。
 * @param add
 */
void updateIndexCacheInfo(Index &index, FileMode mode, const char *hash, std::filesystem::path path, bool add=false);