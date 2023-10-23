#include <gtest/gtest.h>

#include "GitHashObject.hpp"
#include "objects_proxy.hpp"

using namespace std::string_view_literals;

TEST(HashObjectTest, TestSSO) {
    EXPECT_TRUE(testSSO(false));
}

TEST(HashObjectTest, TestRawStringview) {
    EXPECT_EQ(hashObject("blob 11\0Hello world"sv), "70c379b63ffa0795fdbfbc128e5a2818397b7ef8"sv);
}

TEST(HashObjectTest, TestRawStringview1) {
    EXPECT_EQ(hashObject("123"), "40bd001563085fc35165329ea1ff5c5ecbdbbeef"sv);
}

TEST(HashObjectTest, TestRawStringview2) {
    EXPECT_EQ(hashObject("vbao"), "58a2c499e1947eed859296fd99f49d161d1c08ba"sv);
}

TEST(HashObjectTest, TestStringInput) {
    EXPECT_EQ(hashObjectInterface("Hello world"), "70c379b63ffa0795fdbfbc128e5a2818397b7ef8"sv);
}

TEST(HashObjectTest, TestWriteBlobObject) {
    using namespace std::string_view_literals;
    StringView expect = "70c379b63ffa0795fdbfbc128e5a2818397b7ef8"sv;

    EXPECT_EQ(expect, hashObjectInterface("Hello world"));
    EXPECT_EQ(expect, hashObjectInterface("Hello world", InArgType::kRawString, ObjectType::kBlob, true));

    Path path = GitObjectsProxy::getObjectsDir() / Path(expect);
    ASSERT_TRUE(std::filesystem::exists(path));
    std::string content = readFile(path);
    EXPECT_NE(content.size(), strlen("Hello world"));
    EXPECT_EQ(content.size(), 19);
    EXPECT_EQ(std::string_view(content), "blob 11\0Hello world"sv);
}

TEST(HashObjectTest, TestWriteTreeObjectEmpty) {
    using namespace std::string_view_literals;
    StringView expect = "4b825dc642cb6eb9a060e54bf8d69288fbee4904"sv;

    Tree tree;
    EXPECT_EQ(expect, hashObjectInterface(tree.freeze(), InArgType::kRawString, ObjectType::kTree, true));

    Path path = GitObjectsProxy::getObjectsDir() / Path(expect);
    ASSERT_TRUE(std::filesystem::exists(path));
    std::string content = readFile(path);
    EXPECT_EQ(content.size(), 7);
    EXPECT_EQ(std::string_view(content), "tree 0\0"sv);
}

TEST(HashObjectTest, TestWriteTreeObject) {
    using namespace std::string_view_literals;
    StringView expect = "d2a17cf8ec20d54159d656367b3b6de8f5ff9668"sv;

    Tree tree;
    const char hello_sha[] = "70c379b63ffa0795fdbfbc128e5a2818397b7ef8";
    tree.addItem(TreeItem("1.txt", hello_sha, FileMode::kRegular, ObjectType::kBlob));
    EXPECT_EQ(expect, hashObjectInterface(tree.freeze(), InArgType::kRawString, ObjectType::kTree, true));

    Path path = GitObjectsProxy::getObjectsDir() / Path(expect);
    ASSERT_TRUE(std::filesystem::exists(path));
    std::string content = readFile(path);
    EXPECT_EQ(content.size(), 41);

    // 比起ObjectsTest检查freeze()，此处额外检查header
    std::stringstream ss;
    ss << "tree 33" << '\0' << "100644 1.txt" << '\0' << SHA1Proxy::compressSHA(hello_sha).data();
    EXPECT_EQ(content, ss.str());
}