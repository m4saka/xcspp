#include <gtest/gtest.h>
#include <xcspp/xcspp.hpp>

TEST(ConditionTest, ConstructWithVector)
{
    const xcspp::Symbol zero(0);
    const xcspp::Symbol one(1);
    const xcspp::Symbol dontCare('#');

    // "01##"
    const xcspp::Condition cond({ zero, one, dontCare, dontCare });
    EXPECT_TRUE(cond.matches({ 0, 1, 0, 0 }));
    EXPECT_TRUE(cond.matches({ 0, 1, 0, 1 }));
    EXPECT_TRUE(cond.matches({ 0, 1, 1, 0 }));
    EXPECT_TRUE(cond.matches({ 0, 1, 1, 1 }));
    EXPECT_FALSE(cond.matches({ 0, 0, 1, 1 }));
    EXPECT_FALSE(cond.matches({ 1, 1, 1, 1 }));
    EXPECT_FALSE(cond.matches({ 1, 0, 1, 1 }));
    EXPECT_EQ(cond.dontCareCount(), 2);
}

TEST(ConditionTest, ConstructWithString)
{
    const xcspp::Condition cond("01##");
    EXPECT_TRUE(cond.matches({ 0, 1, 0, 0 }));
    EXPECT_TRUE(cond.matches({ 0, 1, 0, 1 }));
    EXPECT_TRUE(cond.matches({ 0, 1, 1, 0 }));
    EXPECT_TRUE(cond.matches({ 0, 1, 1, 1 }));
    EXPECT_FALSE(cond.matches({ 0, 0, 1, 1 }));
    EXPECT_FALSE(cond.matches({ 1, 1, 1, 1 }));
    EXPECT_FALSE(cond.matches({ 1, 0, 1, 1 }));
    EXPECT_EQ(cond.dontCareCount(), 2);
}

TEST(ConditionTest, Comparison)
{
    const xcspp::Symbol zero(0);
    const xcspp::Symbol one(1);
    const xcspp::Symbol dontCare('#');

    // "01##"
    const xcspp::Condition cond1_1({ zero, one, dontCare, dontCare });
    const xcspp::Condition cond1_2("01##");

    // "01#1"
    const xcspp::Condition cond2_1({ zero, one, dontCare, one });
    const xcspp::Condition cond2_2("01#1");

    // ==
    EXPECT_EQ(cond1_1, cond1_2);
    EXPECT_EQ(cond2_1, cond2_2);

    // !=
    EXPECT_NE(cond1_1, cond2_1);
    EXPECT_NE(cond1_1, cond2_2);
    EXPECT_NE(cond1_2, cond2_1);
    EXPECT_NE(cond1_2, cond2_2);
}
