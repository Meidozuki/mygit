#include <gtest/gtest.h>

#include <type_traits>
using namespace std;

#include "sha1_proxy.hpp"
#include "common.hpp"

TEST(CheckTypeProperty, CheckShaStringTrivial) {
    // SHAString is expected to be trivially-copyable
    // as it will be frequently used in function calls
    EXPECT_TRUE(is_trivially_copyable_v<SHAString>);
}


TEST(CheckTypeProperty, CheckConditionCheckTrivial) {
    EXPECT_TRUE(is_trivially_copyable_v<ConditionCheck>);
    EXPECT_TRUE(is_trivial_v<ConditionCheck>);
}