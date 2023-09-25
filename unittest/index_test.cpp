#include <gtest/gtest.h>

#include <fstream>
#include <string_view>
using namespace std::string_view_literals;

#include "index.hpp"
#include "objects_proxy.hpp"

class IndexTest: public ::testing::Test {
 protected:
    class IndexWrapper: public Index {
     public:
        IndexWrapper():Index() {suppress_warning=true;}
    };

    IndexWrapper index_;

    Index& index() {
        return index_;
    }

    Path index_path;
    std::string index_file_standard;
    
    IndexTest():index_(),index_path(GitObjectsProxy::getGitDir() / "index") {}

    void SetUp() override {
        index().clean();

        if (std::filesystem::exists(index_path)) {
            std::filesystem::remove(index_path);
        }

        index_file_standard = "DIRC\n"
                              "100644 blob 0 sha111\n"
                              "1.txt\n"
                              "100644 blob 0 sha222\n"
                              "2.txt\n"
                              "100644 blob 0 sha333\n"
                              "3.txt\n";
    }

    void indexOperation() {
        index().addEntry<DryRunFile>(DryRunFile("sha111","1.txt"));
        index().addEntry<DryRunFile>(DryRunFile("sha222","2.txt"));
        index().addEntry<DryRunFile>(DryRunFile("sha333","3.txt"));
    }

    void writeToIndex(const char* content) {
        std::ofstream ofs(index_path);
        ASSERT_TRUE(ofs.is_open());
        ofs << content;
        ofs.close();
        ASSERT_TRUE(std::filesystem::exists(index_path));
    }
};

TEST_F(IndexTest, Test1) {
    index().addEntry(DryRunFile("sha111","1.txt"));
    for (auto &[k,ptr]: Index::getInstance().getDict()) {
        EXPECT_EQ(k, "1.txt");
        EXPECT_EQ(ptr->sha1, "sha111"sv);
    }
}

TEST_F(IndexTest, Test2) {
    index().addEntry(DryRunFile("sha222333","2.txt"));
    for (auto &[k,ptr]: Index::getInstance().getDict()) {
        EXPECT_EQ(k, "2.txt");
        EXPECT_EQ(ptr->sha1, "sha222333"sv);
    }
}


TEST_F(IndexTest, TestIndexOutput) {
    indexOperation();
    std::ofstream ofs(index_path);
    ASSERT_TRUE(ofs.is_open());
    ofs << index();
    ofs.close();

    std::string s=readFile(index_path);
    ASSERT_EQ(s, index_file_standard);
}

TEST_F(IndexTest, TestIndexIO) {
    indexOperation();
    std::ofstream ofs(index_path);
    ASSERT_TRUE(ofs.is_open());
    ofs << index();
    ofs.close();
    ASSERT_TRUE(std::filesystem::exists(index_path));

    std::string s=readFile(index_path);
    ASSERT_EQ(s, index_file_standard);

    ASSERT_TRUE(index().initFromDisk(index_path));

    Path key_standard[] = {"1.txt", "2.txt", "3.txt"};
    SHAString value_standard[] = {"sha111"sv, "sha222"sv, "sha333"sv};

    int i=0;
    for (auto &[k,ptr]: index().getDict()) {
        EXPECT_EQ(k, key_standard[i]);
        EXPECT_EQ(ptr->sha1, value_standard[i]);
        i++;
    }
    ASSERT_EQ(i, 3);
}

TEST_F(IndexTest, TestIndexInputSucessEmpty) {
    writeToIndex("DIRC\n");

    ASSERT_TRUE(index().initFromDisk(index_path));
    ASSERT_EQ(index().getDict().size(), 0);
}

TEST_F(IndexTest, TestIndexInputSucessRegular) {
    writeToIndex("DIRC\n" "100644 blob 0 sha111\n" "1.txt\n");

    ASSERT_TRUE(index().initFromDisk(index_path));
    ASSERT_EQ(index().getDict().size(), 1);
    
    for (auto &[k,ptr]: Index::getInstance().getDict()) {
        EXPECT_EQ(k, "1.txt");
        EXPECT_EQ(ptr->sha1, "sha111"sv);
    }
}

TEST_F(IndexTest, TestIndexInputSucessRegularExe) {
    writeToIndex("DIRC\n" "100755 blob 0 sha111\n" "1.txt\n");

    ASSERT_TRUE(index().initFromDisk(index_path));
    ASSERT_EQ(index().getDict().size(), 1);
    
    for (auto &[k,ptr]: Index::getInstance().getDict()) {
        EXPECT_EQ(k, "1.txt");
        EXPECT_EQ(ptr->sha1, "sha111"sv);
    }
}

TEST_F(IndexTest, TestIndexInputSucessRegular2) {
    writeToIndex(index_file_standard.c_str());

    ASSERT_TRUE(index().initFromDisk(index_path));
    ASSERT_EQ(index().getDict().size(), 3);
}

TEST_F(IndexTest, TestIndexInputFailureNoHeader) {
    writeToIndex("100644 blob 0 sha111\n" "1.txt\n");

    ASSERT_FALSE(index().initFromDisk(index_path));
}

TEST_F(IndexTest, TestIndexInputFailureWrongFileMode) {
    // 777 is valid in UNIX but not in Git system
    writeToIndex("DIRC\n" "100777 blob 0 sha111\n" "1.txt\n");

    ASSERT_FALSE(index().initFromDisk(index_path));
}

TEST_F(IndexTest, TestIndexInputFailureWrongObjectType) {
    writeToIndex("DIRC\n" "100644 commit 0 sha111\n" "1.txt\n");

    ASSERT_FALSE(index().initFromDisk(index_path));
    ASSERT_EQ(index().getDict().size(), 0);
}

TEST_F(IndexTest, TestIndexInputFailureWrongObjectType2) {
    writeToIndex("DIRC\n" 
        "100644 commit 0 sha111\n" "1.txt\n"
        "100644 blob 0 sha222\n" "2.txt\n");

    ASSERT_FALSE(index().initFromDisk(index_path));
    ASSERT_EQ(index().getDict().size(), 0);
}

TEST_F(IndexTest, TestIndexInputFailureWrongObjectType3) {
    writeToIndex("DIRC\n" "100644 1 0 sha111\n" "1.txt\n");

    ASSERT_FALSE(index().initFromDisk(index_path));
    ASSERT_EQ(index().getDict().size(), 0);
}

TEST_F(IndexTest, TestIndexInputFailureWrongFilename) {
    writeToIndex("DIRC\n" "100644 blob 0 sha111\n" "\n");

    ASSERT_FALSE(index().initFromDisk(index_path));
    ASSERT_EQ(index().getDict().size(), 0);
}