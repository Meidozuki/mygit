#include <gtest/gtest.h>

#include <filesystem>
#include <string_view>
using namespace std::string_view_literals;

#include "GitUpdateIndex.hpp"

#include "config.h"
#include "index_test_common.hpp"

class WriteTreeTest: public IndexTestBase{
 protected:
    void SetUp() override{
        cleanMemory();
        // cleanDisk();
    }
};

TEST_F(WriteTreeTest, TestHelloWorldByCacheInfo) {
    Path filename = "1.txt";

    writeToFile(filename,"Hello world"); // To suppress warning by update-index cacheinfo
    hashObjectInterface("Hello world", InArgType::kRawString, ObjectType::kBlob, true);
    updateIndexCacheInfo(index(),FileMode::kRegular,"70c379b63ffa0795fdbfbc128e5a2818397b7ef8","1.txt",true);
    removeFile(filename);

    EXPECT_EQ(index().getDict().size(), 1);
    for (auto &[_,ptr]: index().getDict()) {
        EXPECT_TRUE(isRegularFile_loose(ptr->file_mode));
        EXPECT_EQ(ptr->file_size, 0);
        EXPECT_STREQ(ptr->sha1, "70c379b63ffa0795fdbfbc128e5a2818397b7ef8");
        EXPECT_EQ(ptr->filename, filename);
    }

    EXPECT_EQ(index().writeTree(false), "d2a17cf8ec20d54159d656367b3b6de8f5ff9668"sv);
}

TEST_F(WriteTreeTest, TestHelloWorldByFileReadOnly) {
    Path filename = "1.txt";
    writeToFile(filename,"Hello world");
    chmod644(filename);

    updateIndex(index(), filename);
#ifdef Windows_file_permission
    EXPECT_TRUE(index().chmod(filename, false));
#endif

    EXPECT_EQ(index().getDict().size(), 1);
    for (auto &[_,ptr]: index().getDict()) {
        EXPECT_EQ(ptr->file_mode, FileMode::kRegular);
        EXPECT_NE(ptr->file_size, 0);
        EXPECT_STREQ(ptr->sha1, "70c379b63ffa0795fdbfbc128e5a2818397b7ef8");
        EXPECT_EQ(ptr->filename, filename);
    }

    EXPECT_EQ(index().writeTree(false), "d2a17cf8ec20d54159d656367b3b6de8f5ff9668"sv);

    removeFile(filename);
}

TEST_F(WriteTreeTest, TestHelloWorldByFileExecutable) {
    Path filename = "1.txt";
    writeToFile(filename,"Hello world");
    chmod755(filename);

    updateIndex(index(),filename);
    EXPECT_TRUE(index().chmod(filename, true));

    EXPECT_EQ(index().getDict().size(), 1);
    for (auto &[_,ptr]: index().getDict()) {
        EXPECT_EQ(ptr->file_mode, FileMode::kRegularExecutable);
        EXPECT_NE(ptr->file_size, 0);
        EXPECT_STREQ(ptr->sha1, "70c379b63ffa0795fdbfbc128e5a2818397b7ef8");
        EXPECT_EQ(ptr->filename, filename);
    }

    EXPECT_EQ(index().writeTree(false), "8a82beef2498ae1c681a4c2371b46b2a4930101a"sv);

    removeFile(filename);
}

TEST_F(WriteTreeTest, TestTwoLevelDirectory) {
    Path filename = "folder/1.txt";
    writeToFile(filename,"Hello world");
    chmod644(filename);

    updateIndex(index(), filename);
//#ifdef Windows_file_permission
    EXPECT_TRUE(index().chmod(filename, false));
//#endif

    EXPECT_EQ(index().getDict().size(), 1);
    for (auto &[_,ptr]: index().getDict()) {
        EXPECT_EQ(ptr->file_mode, FileMode::kRegular);
        EXPECT_NE(ptr->file_size, 0);
        EXPECT_STREQ(ptr->sha1, "70c379b63ffa0795fdbfbc128e5a2818397b7ef8");
        EXPECT_EQ(ptr->filename, filename);
    }

    EXPECT_EQ(index().writeTree(false), "5fb4ed5210aa767bf8877afe979febb3a80fb0bd"sv);

    std::ofstream(index_path) << index();
    removeFile(filename);
}
