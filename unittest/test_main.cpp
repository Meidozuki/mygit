#include <gtest/gtest.h>

#include "./hash_object_test.cpp"

#include "index.hpp"

TEST(IndexTest, Test1) {
//    Index::getInstance().addEntry<DryRunFile>(DryRunFile("sha111","1.txt"));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}