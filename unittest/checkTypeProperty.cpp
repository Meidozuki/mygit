#include <gtest/gtest.h>

#include <type_traits>
using namespace std;

#include "sha1_proxy.hpp"
#include "common.hpp"

TEST(CheckTypeProperty, CheckTrivial_ShaString) {
    // SHAString is expected to be trivially-copyable
    // as it will be frequently used in function calls
    EXPECT_TRUE(is_trivially_copyable_v<SHAString>);
}


TEST(CheckTypeProperty, CheckTrivial_ConditionCheck) {
    EXPECT_TRUE(is_trivially_copyable_v<ConditionCheck>);
    EXPECT_TRUE(is_trivial_v<ConditionCheck>);
}

TEST(CheckTypeProperty, CheckTrivial_ObjectType) {
    EXPECT_TRUE(is_trivially_copyable_v<ObjectType>);
    EXPECT_TRUE(is_trivial_v<ObjectType>);
    using Optioned = std::optional<ObjectType>;
    EXPECT_TRUE(is_trivially_copyable_v<Optioned>);
}