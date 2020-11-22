#include <gtest/gtest.h>
#include <xcspp/xcspp.hpp>

using namespace xcspp;

TEST(ConditionTest, ConstructWithVector)
{
    const xcs::Symbol zero(0);
    const xcs::Symbol one(1);
    const xcs::Symbol dontCare('#');

    // "01##"
    const xcs::Condition cond({ zero, one, dontCare, dontCare });
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
    const xcs::Condition cond("0 1 # #");
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
    const xcs::Symbol zero(0);
    const xcs::Symbol one(1);
    const xcs::Symbol dontCare('#');

    // "01##"
    const xcs::Condition cond1_1({ zero, one, dontCare, dontCare });
    const xcs::Condition cond1_2("0 1 # #");

    // "01#1"
    const xcs::Condition cond2_1({ zero, one, dontCare, one });
    const xcs::Condition cond2_2("0 1 # 1");

    // ==
    EXPECT_EQ(cond1_1, cond1_2);
    EXPECT_EQ(cond2_1, cond2_2);

    // !=
    EXPECT_NE(cond1_1, cond2_1);
    EXPECT_NE(cond1_1, cond2_2);
    EXPECT_NE(cond1_2, cond2_1);
    EXPECT_NE(cond1_2, cond2_2);
}

TEST(ConditionTest, IsMoreGeneral)
{
    const xcs::Condition cond1("0 1 # #");
    const xcs::Condition cond2("0 1 # 1");
    const xcs::Condition cond3("1 1 # #");

    EXPECT_FALSE(cond1.isMoreGeneral(cond1));
    EXPECT_FALSE(cond2.isMoreGeneral(cond2));
    EXPECT_FALSE(cond3.isMoreGeneral(cond3));

    // "01##" vs. "01#1"
    EXPECT_TRUE(cond1.isMoreGeneral(cond2));
    EXPECT_FALSE(cond2.isMoreGeneral(cond1));

    // "01##" vs. "11##"
    EXPECT_FALSE(cond1.isMoreGeneral(cond3));
    EXPECT_FALSE(cond3.isMoreGeneral(cond1));

    // "01#1" vs. "11##"
    EXPECT_FALSE(cond2.isMoreGeneral(cond3));
    EXPECT_FALSE(cond3.isMoreGeneral(cond2));

    // vs. "####"
    const xcs::Condition allDontCare("# # # #");
    EXPECT_TRUE(allDontCare.isMoreGeneral(cond1));
    EXPECT_TRUE(allDontCare.isMoreGeneral(cond2));
    EXPECT_TRUE(allDontCare.isMoreGeneral(cond3));
    EXPECT_FALSE(cond1.isMoreGeneral(allDontCare));
    EXPECT_FALSE(cond2.isMoreGeneral(allDontCare));
    EXPECT_FALSE(cond3.isMoreGeneral(allDontCare));
}

TEST(ConditionTest, ToStringAndConstructor)
{
    // Original strings
    const std::string condStr1 = "0 1 # #";
    const std::string condStr2 = "0 1 # 1";
    const std::string condStr3 = "1 1 # #";

    // Generated conditions
    const xcs::Condition cond1(condStr1);
    const xcs::Condition cond2(condStr2);
    const xcs::Condition cond3(condStr3);

    // Reconstructed strings
    const std::string condStrRec1 = cond1.toString();
    const std::string condStrRec2 = cond2.toString();
    const std::string condStrRec3 = cond3.toString();

    EXPECT_EQ(condStr1, condStrRec1);
    EXPECT_EQ(condStr2, condStrRec2);
    EXPECT_EQ(condStr3, condStrRec3);
}
