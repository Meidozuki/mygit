#include <gtest/gtest.h>

#include "hash_object_test.cpp"
#include "index_test.cpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}