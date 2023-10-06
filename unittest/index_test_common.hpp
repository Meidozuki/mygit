#pragma once

#include <filesystem>
#include <type_traits>

#include <gtest/gtest.h>

#include "index.hpp"
#include "common.hpp"

class IndexTestBase: public ::testing::Test {
 private:
    using Dict = std::remove_reference_t<decltype(Index::getInstance().getDict())>;
 protected:
    using Key = Dict::key_type;
    using Value = Dict::mapped_type;

    Path index_path;

    IndexTestBase():index_path(GitObjectsProxy::getGitDir() / "index") {}

    virtual Index& index() {
        return Index::getInstance();
    }

    void cleanMemory() {
        index().clean();
    }

    void cleanDisk() {
        if (filesys::exists(index_path)) {
            filesys::remove(index_path);
        }
    }

    void compareOneEntry(const Key& key, const char* value) {
        EXPECT_EQ(index().getDict().size(), 1);
        for (auto &[dict_key,ptr]: index().getDict()) {
            EXPECT_EQ(dict_key, key);
            EXPECT_STREQ(ptr->sha1, value);
        }
    }

    void writeToFile(Path filename, std::string content) {
        if (Path parent = filename.parent_path();
            parent != "" && !filesys::exists(parent)) {
            filesys::create_directories(parent);
        }
        std::ofstream(filename) << content;
        ASSERT_TRUE(std::filesystem::exists(filename));
    }

    void removeFile(const Path &path) {
        if (filesys::exists(path)) {
            filesys::remove(path);
        }
    }

    bool isRegularFile_loose(FileMode mode) {
        return mode == FileMode::kRegular || mode == FileMode::kRegularExecutable;
    }

    bool isRegularFile(FileMode mode) {
        return mode == FileMode::kRegular;
    }
};
