#include <gtest/gtest.h>

#include "GitHashObject.hpp"

TEST(HashTest, TestRawStringview) {
    using namespace std::string_view_literals;
    EXPECT_EQ(hashObject("blob 11\0Hello world"sv), "70c379b63ffa0795fdbfbc128e5a2818397b7ef8"sv);
}

TEST(HashTest, TestStringInput) {
    EXPECT_EQ(hashObjectInterface("Hello world"), "70c379b63ffa0795fdbfbc128e5a2818397b7ef8");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}