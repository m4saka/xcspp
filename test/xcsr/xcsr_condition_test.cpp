#include <gtest/gtest.h>
#include <xcspp/xcspp.hpp>

using namespace xcspp;

TEST(XCSR_ConditionTest, ConstructWithVector)
{
    const xcsr::Symbol zero(0.0, 0.5);
    const xcsr::Symbol one(0.5, 1.0);
    const xcsr::Symbol dontCare(0.0, 1.0);

    // "01##"
    const xcsr::Condition cond({ zero, one, dontCare, dontCare });
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.25, 0.25 }, xcsr::XCSRRepr::kOBR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.25, 0.75 }, xcsr::XCSRRepr::kOBR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.75, 0.25 }, xcsr::XCSRRepr::kOBR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.75, 0.75 }, xcsr::XCSRRepr::kOBR));
    EXPECT_FALSE(cond.matches({ 0.25, 0.25, 0.75, 0.75 }, xcsr::XCSRRepr::kOBR));
    EXPECT_FALSE(cond.matches({ 0.75, 0.75, 0.75, 0.75 }, xcsr::XCSRRepr::kOBR));
    EXPECT_FALSE(cond.matches({ 0.75, 0.25, 0.75, 0.75 }, xcsr::XCSRRepr::kOBR));
}

TEST(XCSR_ConditionTest, ConstructWithString)
{
    // "01##"
    const xcsr::Condition cond("0.0;0.5 0.5;1.0 0.0;1.0 0.0;1.0");
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.25, 0.25 }, xcsr::XCSRRepr::kOBR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.25, 0.75 }, xcsr::XCSRRepr::kOBR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.75, 0.25 }, xcsr::XCSRRepr::kOBR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.75, 0.75 }, xcsr::XCSRRepr::kOBR));
    EXPECT_FALSE(cond.matches({ 0.25, 0.25, 0.75, 0.75 }, xcsr::XCSRRepr::kOBR));
    EXPECT_FALSE(cond.matches({ 0.75, 0.75, 0.75, 0.75 }, xcsr::XCSRRepr::kOBR));
    EXPECT_FALSE(cond.matches({ 0.75, 0.25, 0.75, 0.75 }, xcsr::XCSRRepr::kOBR));
}

TEST(XCSR_ConditionTest, Comparison)
{
    const xcsr::Symbol zero(0.0, 0.5);
    const xcsr::Symbol one(0.5, 1.0);
    const xcsr::Symbol dontCare(0.0, 1.0);

    // "01##"
    const xcsr::Condition cond1_1({ zero, one, dontCare, dontCare });
    const xcsr::Condition cond1_2("0.0;0.5 0.5;1.0 0.0;1.0 0.0;1.0");

    // "01#1"
    const xcsr::Condition cond2_1({ zero, one, dontCare, one });
    const xcsr::Condition cond2_2("0.0;0.5 0.5;1.0 0.0;1.0 0.5;1.0");

    // ==
    EXPECT_EQ(cond1_1, cond1_2);
    EXPECT_EQ(cond2_1, cond2_2);

    // !=
    EXPECT_NE(cond1_1, cond2_1);
    EXPECT_NE(cond1_1, cond2_2);
    EXPECT_NE(cond1_2, cond2_1);
    EXPECT_NE(cond1_2, cond2_2);
}

TEST(XCSR_ConditionTest, IsMoreGeneral)
{
    const xcsr::Condition cond1("0.0;0.5 0.5;1.0 0.0;1.0 0.0;1.0");
    const xcsr::Condition cond2("0.0;0.5 0.5;1.0 0.0;1.0 0.5;1.0");
    const xcsr::Condition cond3("0.5;1.0 0.5;1.0 0.0;1.0 0.0;1.0");

    EXPECT_FALSE(cond1.isMoreGeneral(cond1, xcsr::XCSRRepr::kOBR));
    EXPECT_FALSE(cond2.isMoreGeneral(cond2, xcsr::XCSRRepr::kOBR));
    EXPECT_FALSE(cond3.isMoreGeneral(cond3, xcsr::XCSRRepr::kOBR));

    // "01##" vs. "01#1"
    EXPECT_TRUE(cond1.isMoreGeneral(cond2, xcsr::XCSRRepr::kOBR));
    EXPECT_FALSE(cond2.isMoreGeneral(cond1, xcsr::XCSRRepr::kOBR));

    // "01##" vs. "11##"
    EXPECT_FALSE(cond1.isMoreGeneral(cond3, xcsr::XCSRRepr::kOBR));
    EXPECT_FALSE(cond3.isMoreGeneral(cond1, xcsr::XCSRRepr::kOBR));

    // "01#1" vs. "11##"
    EXPECT_FALSE(cond2.isMoreGeneral(cond3, xcsr::XCSRRepr::kOBR));
    EXPECT_FALSE(cond3.isMoreGeneral(cond2, xcsr::XCSRRepr::kOBR));

    // vs. "####"
    const xcsr::Condition allDontCare("0.0;1.0 0.0;1.0 0.0;1.0 0.0;1.0");
    EXPECT_TRUE(allDontCare.isMoreGeneral(cond1, xcsr::XCSRRepr::kOBR));
    EXPECT_TRUE(allDontCare.isMoreGeneral(cond2, xcsr::XCSRRepr::kOBR));
    EXPECT_TRUE(allDontCare.isMoreGeneral(cond3, xcsr::XCSRRepr::kOBR));
    EXPECT_FALSE(cond1.isMoreGeneral(allDontCare, xcsr::XCSRRepr::kOBR));
    EXPECT_FALSE(cond2.isMoreGeneral(allDontCare, xcsr::XCSRRepr::kOBR));
    EXPECT_FALSE(cond3.isMoreGeneral(allDontCare, xcsr::XCSRRepr::kOBR));
}
