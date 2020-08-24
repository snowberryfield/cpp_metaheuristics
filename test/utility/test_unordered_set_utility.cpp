/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <string>

#include <cppmh.h>

namespace {
/*****************************************************************************/
class TestUnorderedSetUtility : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestUnorderedSetUtility, intersection_set) {
    std::unordered_set<int>         set_int_0    = {1, 2, 3, 4, 5};
    std::unordered_set<int>         set_int_1    = {3, 4, 5, 6, 7};
    std::unordered_set<std::string> set_string_0 = {"a", "b", "c", "d", "e"};
    std::unordered_set<std::string> set_string_1 = {"c", "d", "e", "f", "g"};

    auto intersection_int =
        cppmh::utility::intersection_set(set_int_0, set_int_1);

    auto intersection_str =
        cppmh::utility::intersection_set(set_string_0, set_string_1);

    EXPECT_EQ(false, intersection_int.find(1) != intersection_int.end());
    EXPECT_EQ(false, intersection_int.find(2) != intersection_int.end());
    EXPECT_EQ(true, intersection_int.find(3) != intersection_int.end());
    EXPECT_EQ(true, intersection_int.find(4) != intersection_int.end());
    EXPECT_EQ(true, intersection_int.find(5) != intersection_int.end());
    EXPECT_EQ(false, intersection_int.find(6) != intersection_int.end());
    EXPECT_EQ(false, intersection_int.find(7) != intersection_int.end());

    EXPECT_EQ(false, intersection_str.find("a") != intersection_str.end());
    EXPECT_EQ(false, intersection_str.find("b") != intersection_str.end());
    EXPECT_EQ(true, intersection_str.find("c") != intersection_str.end());
    EXPECT_EQ(true, intersection_str.find("d") != intersection_str.end());
    EXPECT_EQ(true, intersection_str.find("e") != intersection_str.end());
    EXPECT_EQ(false, intersection_str.find("f") != intersection_str.end());
    EXPECT_EQ(false, intersection_str.find("g") != intersection_str.end());
}

/*****************************************************************************/
TEST_F(TestUnorderedSetUtility, union_set) {
    std::unordered_set<int>         set_int_0    = {1, 2, 3, 4, 5};
    std::unordered_set<int>         set_int_1    = {3, 4, 5, 6, 7};
    std::unordered_set<std::string> set_string_0 = {"a", "b", "c", "d", "e"};
    std::unordered_set<std::string> set_string_1 = {"c", "d", "e", "f", "g"};

    auto union_int = cppmh::utility::union_set(set_int_0, set_int_1);
    auto union_str = cppmh::utility::union_set(set_string_0, set_string_1);

    EXPECT_EQ(true, union_int.find(1) != union_int.end());
    EXPECT_EQ(true, union_int.find(2) != union_int.end());
    EXPECT_EQ(true, union_int.find(3) != union_int.end());
    EXPECT_EQ(true, union_int.find(4) != union_int.end());
    EXPECT_EQ(true, union_int.find(5) != union_int.end());
    EXPECT_EQ(true, union_int.find(6) != union_int.end());
    EXPECT_EQ(true, union_int.find(7) != union_int.end());

    EXPECT_EQ(true, union_str.find("a") != union_str.end());
    EXPECT_EQ(true, union_str.find("b") != union_str.end());
    EXPECT_EQ(true, union_str.find("c") != union_str.end());
    EXPECT_EQ(true, union_str.find("d") != union_str.end());
    EXPECT_EQ(true, union_str.find("e") != union_str.end());
    EXPECT_EQ(true, union_str.find("f") != union_str.end());
    EXPECT_EQ(true, union_str.find("g") != union_str.end());
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/