#include <gtest/gtest.h>

#include <filesystem>
#include <string_view>
using namespace std::string_view_literals;

#include "index.hpp"
#include "objects_proxy.hpp"
#include "GitUpdateIndex.hpp"

#include "index_test_common.hpp"

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

TEST_F(UpdateIndexTest, TestFailureFileNotExists) {
    removeFile("1.txt");
    EXPECT_ANY_THROW(updateIndex(index(), "1.txt"));
}

class WriteTreeTest: public IndexTestBase{
 protected:
    void SetUp() override{
        cleanMemory();
        cleanDisk();
    }
};

TEST_F(WriteTreeTest, TestHelloWorldByCacheInfo) {
    Path filename = "1.txt";

    hashObjectInterface("Hello world", InArgType::kRawString, ObjectType::kBlob, true);
    updateIndexCacheInfo(index(),FileMode::kRegular,"70c379b63ffa0795fdbfbc128e5a2818397b7ef8","1.txt",true);
    // this will print a warning

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
    using std::filesystem::perms;
    Path filename = "1.txt";
    writeToFile(filename,"Hello world");
//    filesys::permissions(filename,
//                         perms::owner_read | perms::owner_write | perms::group_read | perms::others_read,
//                         filesys::perm_options::replace);

    updateIndex(index(), filename);

    EXPECT_EQ(index().getDict().size(), 1);
    for (auto &[_,ptr]: index().getDict()) {
        EXPECT_TRUE(isRegularFile(ptr->file_mode));
        EXPECT_NE(ptr->file_size, 0);
        EXPECT_STREQ(ptr->sha1, "70c379b63ffa0795fdbfbc128e5a2818397b7ef8");
        EXPECT_EQ(ptr->filename, filename);
    }

    EXPECT_EQ(index().writeTree(false), "d2a17cf8ec20d54159d656367b3b6de8f5ff9668"sv);

    removeFile(filename);
}

TEST_F(WriteTreeTest, TestHelloWorldByFileExecutable) {
    using std::filesystem::perms;
    Path filename = "1.txt";
    writeToFile(filename,"Hello world");
//    filesys::permissions(filename,
//                         perms::owner_exec | perms::group_exec | perms::others_exec,
//                         filesys::perm_options::add);

    updateIndex(index(),filename);

    EXPECT_EQ(index().getDict().size(), 1);
    for (auto &[_,ptr]: index().getDict()) {
        ptr->chmod(true);

        EXPECT_EQ(ptr->file_mode, FileMode::kRegularExecutable);
        EXPECT_NE(ptr->file_size, 0);
        EXPECT_STREQ(ptr->sha1, "70c379b63ffa0795fdbfbc128e5a2818397b7ef8");
        EXPECT_EQ(ptr->filename, filename);
    }

    EXPECT_EQ(index().writeTree(false), "8a82beef2498ae1c681a4c2371b46b2a4930101a"sv);

    removeFile(filename);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}