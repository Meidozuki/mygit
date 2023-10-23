#include <gtest/gtest.h>

#include <string_view>
using namespace std::string_view_literals;

#include "GitHashObject.hpp"
#include "objects_proxy.hpp"

class ObjectsProxyTest: public ::testing::Test {
 protected:
    struct Wrapper: public GitObjectsProxy{
     public:
        using GitObjectsProxy::GitObjectsProxy;
        using GitObjectsProxy::insertNoCheck;
        using GitObjectsProxy::getFilePathNoCheck;
        using GitObjectsProxy::database_;
    };

    Wrapper proxy_;
    Wrapper &proxy() {
        return proxy_;
    }

    SHAString hello_sha;

    void SetUp() override {
        hello_sha = "70c379b63ffa0795fdbfbc128e5a2818397b7ef8";
    }

    void compareOneElement(SHAString hash) {
        auto &db = proxy().database_;
        ASSERT_EQ(db.size(), 1);
        auto it = db.find(hash);
        ASSERT_TRUE(it != db.end());
        ASSERT_EQ(*it, hash);
    }

    void removeObject(SHAString hash) {
        Path path = proxy().getFilePathNoCheck(hash);
        if (filesys::exists(path)) {
            filesys::remove(path);
        }
    }

    void writeFile() {
        hashObjectInterface("Hello world", InArgType::kRawString, ObjectType::kBlob, true);
    }

};

TEST(ObjectsTest, TestObjectTypeNameConvert) {
    EXPECT_STREQ(GitObject::typeName(ObjectType::kBlob), "blob");
    EXPECT_STREQ(GitObject::typeName(ObjectType::kTree), "tree");
    EXPECT_STREQ(GitObject::typeName(ObjectType::kCommit), "commit");
    EXPECT_STREQ(GitObject::typeName(ObjectType::kTag), "tag");
}

TEST(ObjectsTest, TestBlob) {
    Blob obj;
    obj.content_ = "vbao";
    EXPECT_EQ(obj.type_, ObjectType::kBlob);
    EXPECT_EQ(obj.freeze(), "vbao");
}

TEST(ObjectsTest, TestTree) {
    Tree obj;
    const char hello_sha[] = "70c379b63ffa0795fdbfbc128e5a2818397b7ef8";
    obj.addItem(TreeItem("1.txt", hello_sha, FileMode::kRegular, ObjectType::kBlob));

    EXPECT_EQ(obj.type_, ObjectType::kTree);

    std::stringstream ss;
    ss << "100644 1.txt" << '\0' << SHA1Proxy::compressSHA(hello_sha).data();
    EXPECT_EQ(obj.freeze(), ss.str());
}

TEST(ObjectsTest, TestCommit) {
    Commit obj;
    EXPECT_EQ(obj.type_, ObjectType::kCommit);
    EXPECT_EQ(obj.freeze(),"tree \n"
                           "parent nan\n"
                           "author   0 +0000\n"
                           "committer   0 +0000\n"
                           "\n");
}

TEST_F(ObjectsProxyTest, TestClear) {
    auto &db = proxy().database_;
    db.clear();
    db.insert(hello_sha);
    ASSERT_EQ(db.size(), 1);
    proxy().resetMemory();
    ASSERT_EQ(db.size(), 0);
}

TEST_F(ObjectsProxyTest, TestInsert) {
    proxy().insertNoCheck(hello_sha);
    compareOneElement(hello_sha);
}

TEST_F(ObjectsProxyTest, TestInsertAndCheck) {
    removeObject(hello_sha);
    EXPECT_ANY_THROW(proxy().insert(hello_sha));

    writeFile();
    proxy().insert(hello_sha);
    compareOneElement(hello_sha);
}

TEST_F(ObjectsProxyTest, TestErase) {
    removeObject(hello_sha);
    // hash-object使用的是原来的类，此处也要更改
    GitObjectsProxy &proxy = GitObjectsProxy::getInstance();
    proxy.resetMemory();
    EXPECT_FALSE(proxy.erase(hello_sha));

    writeFile();
    EXPECT_TRUE(proxy.erase(hello_sha));
}

TEST_F(ObjectsProxyTest, TestGetObjectType) {
    writeFile();
    EXPECT_EQ(ObjectType::kBlob, GitObjectsProxy::getInstance().getObjectType(hello_sha));
}

TEST_F(ObjectsProxyTest, TestGetObjectType2) {
    writeFile();
    EXPECT_EQ(ObjectType::kBlob, GitObjectsProxy::getInstance().getObjectTypeNoThrow(hello_sha));
}

TEST_F(ObjectsProxyTest, TestReadObjectNoHeader) {
    writeFile();
    auto re = GitObjectsProxy::getInstance().readObjectNoHeader(hello_sha);
    EXPECT_STREQ(re.value().str().c_str(), "Hello world");
}