#include <gtest/gtest.h>

#include <filesystem>
#include <string_view>

#include "index.hpp"
#include "GitUpdateIndex.hpp"

#include "config.h"
#include "index_test_common.hpp"

using namespace std::string_view_literals;
using std::filesystem::perms;

class UpdateIndexTest: public IndexTestBase {
 protected:
    void SetUp() override{
        cleanMemory();
        cleanDisk();
    }
};

TEST_F(UpdateIndexTest, SimpleTest) {
    writeToFile("empty.txt","");
    updateIndex(index(), "empty.txt");

    compareOneEntry("empty.txt", "e69de29bb2d1d6434b8b29ae775ad8c2e48c5391");

    removeFile("empty.txt");
}

TEST_F(UpdateIndexTest, TestSuccess) {
    writeToFile("hello.txt","Hello world");
    updateIndex(index(), "hello.txt");

    const char* hash = "70c379b63ffa0795fdbfbc128e5a2818397b7ef8";
    compareOneEntry("hello.txt", hash);
    for (auto &[_,ptr]: index().getDict()) {
        EXPECT_TRUE(isRegularFile_loose(ptr->file_mode));
        EXPECT_NE(ptr->file_size, 0);
        EXPECT_STREQ(ptr->sha1, hash);
        EXPECT_STREQ(ptr->filename.data(), "hello.txt");
    }

    removeFile("hello.txt");
}

TEST_F(UpdateIndexTest, TestSuccessInFolder) {
    Path filename = "folder/1.txt";
    writeToFile(filename,"Hello world");
    updateIndex(index(), filename);

    const char* hash = "70c379b63ffa0795fdbfbc128e5a2818397b7ef8";
    compareOneEntry(filename, hash);
    for (auto &[_,ptr]: index().getDict()) {
        EXPECT_TRUE(isRegularFile_loose(ptr->file_mode));
        EXPECT_NE(ptr->file_size, 0);
        EXPECT_STREQ(ptr->sha1, hash);
        EXPECT_EQ(ptr->filename, filename);
    }

    removeFile(filename);
}


TEST_F(UpdateIndexTest, TestSuccessStrictRegular) {
    Path filename = "hello.txt";
    writeToFile(filename,"Hello world");
    chmod644(filename);

    updateIndex(index(), filename);
#ifdef Windows_file_permission
    EXPECT_TRUE(index().chmod(filename, false));
#endif

    const char* hash = "70c379b63ffa0795fdbfbc128e5a2818397b7ef8";
    compareOneEntry(filename, hash);
    for (auto &[_,ptr]: index().getDict()) {
        EXPECT_EQ(ptr->file_mode, FileMode::kRegular);
        EXPECT_NE(ptr->file_size, 0);
        EXPECT_STREQ(ptr->sha1, "70c379b63ffa0795fdbfbc128e5a2818397b7ef8");
        EXPECT_EQ(ptr->filename, filename);
    }

    removeFile(filename);
}

TEST_F(UpdateIndexTest, TestSuccessStrictRegularExe) {
    Path filename = "hello.txt";
    writeToFile(filename,"Hello world");
    chmod755(filename);

    updateIndex(index(), filename);
#ifdef Windows_file_permission
    EXPECT_TRUE(index().chmod(filename, true));
#endif

    const char* hash = "70c379b63ffa0795fdbfbc128e5a2818397b7ef8";
    compareOneEntry(filename, hash);
    for (auto &[_,ptr]: index().getDict()) {
        EXPECT_EQ(ptr->file_mode, FileMode::kRegularExecutable);
        EXPECT_NE(ptr->file_size, 0);
        EXPECT_STREQ(ptr->sha1, "70c379b63ffa0795fdbfbc128e5a2818397b7ef8");
        EXPECT_EQ(ptr->filename, filename);
    }

    removeFile(filename);
}

TEST_F(UpdateIndexTest, TestFailureFileNotExists) {
    removeFile("1.txt");
    EXPECT_ANY_THROW(updateIndex(index(), "1.txt"));
}