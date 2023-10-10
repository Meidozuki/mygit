
#pragma once

#include "precompile.h"

#include <filesystem>

#include "common.hpp"
#include "index.hpp"

void updateIndex(Index &index, const Path &filename, bool add=true, bool remove=false);


/**
 * @param mode
 * @param hash should a valid SHA1 that already adds to Objects Database. (原始git不检查是否存在)
 * @param path 可以为空路径，此时必须指定--add （对应的file size为0）。
 * @param add
 */
void updateIndexCacheInfo(Index &index, FileMode mode, const char *hash, Path filename, bool add=false, bool remove=false);