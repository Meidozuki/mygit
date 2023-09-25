#include <gtest/gtest.h>

#include "GitHashObject.hpp"
#include "objects_proxy.hpp"

TEST(HashObjectTest, TestSSO) {
    EXPECT_TRUE(testSSO(false));
}

TEST(HashObjectTest, TestRawStringview) {
    using namespace std::string_view_literals;
    EXPECT_EQ(hashObject("blob 11\0Hello world"sv), "70c379b63ffa0795fdbfbc128e5a2818397b7ef8");
}

TEST(HashObjectTest, TestRawStringview1) {
    EXPECT_EQ(hashObject("123"), "40bd001563085fc35165329ea1ff5c5ecbdbbeef");
}

TEST(HashObjectTest, TestRawStringview2) {
    EXPECT_EQ(hashObject("vbao"), "58a2c499e1947eed859296fd99f49d161d1c08ba");
}

TEST(HashObjectTest, TestStringInput) {
    EXPECT_EQ(hashObjectInterface("Hello world"), "70c379b63ffa0795fdbfbc128e5a2818397b7ef8");
}

TEST(HashObjectTest, TestFileWrite) {
    using namespace std::string_view_literals;
    const char* expect = "70c379b63ffa0795fdbfbc128e5a2818397b7ef8";
    EXPECT_EQ(hashObjectInterface("Hello world"), expect);
    SHAString hash = hashObjectInterface("Hello world", InArgType::kRawString, ObjectType::kBlob, true);
    EXPECT_EQ(hash, expect);

    Path path = GitObjectsProxy::getObjectsDir() / Path(hash);
    ASSERT_TRUE(std::filesystem::exists(path));
    std::string content = readFile(path);
    EXPECT_NE(content.size(), strlen("Hello world"));
    EXPECT_EQ(content.size(), 19);
    EXPECT_EQ(std::string_view(content), "blob 11\0Hello world"sv);
}