#include <gtest/gtest.h>
#include <xcspp/xcspp.hpp>

using namespace xcspp;

TEST(XCSR_ConditionTest, Comparison)
{
    const xcsr::Symbol zero(0.25, 0.25);
    const xcsr::Symbol one(0.75, 0.25);
    const xcsr::Symbol dontCare(0.5, 0.5);

    // "01##"
    const xcsr::Condition cond1({ zero, one, dontCare, dontCare });

    // "01#1"
    const xcsr::Condition cond2({ zero, one, dontCare, one });

    // ==
    EXPECT_EQ(cond1, cond1);
    EXPECT_EQ(cond2, cond2);

    // !=
    EXPECT_NE(cond1, cond2);
}

TEST(XCSR_ConditionTest, ConstructWithVector_CSR)
{
    const xcsr::Symbol zero(0.25, 0.25);
    const xcsr::Symbol one(0.75, 0.25);
    const xcsr::Symbol dontCare(0.5, 0.5);

    // "01##"
    const xcsr::Condition cond({ zero, one, dontCare, dontCare });
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.25, 0.25 }, XCSRRepr::kCSR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.25, 0.75 }, XCSRRepr::kCSR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.75, 0.25 }, XCSRRepr::kCSR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.75, 0.75 }, XCSRRepr::kCSR));
    EXPECT_FALSE(cond.matches({ 0.25, 0.25, 0.75, 0.75 }, XCSRRepr::kCSR));
    EXPECT_FALSE(cond.matches({ 0.75, 0.75, 0.75, 0.75 }, XCSRRepr::kCSR));
    EXPECT_FALSE(cond.matches({ 0.75, 0.25, 0.75, 0.75 }, XCSRRepr::kCSR));
}

TEST(XCSR_ConditionTest, ConstructWithString_CSR)
{
    // "01##"
    const xcsr::Condition cond("0.25;0.25 0.75;0.25 0.5;0.5 0.5;0.5");
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.25, 0.25 }, XCSRRepr::kCSR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.25, 0.75 }, XCSRRepr::kCSR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.75, 0.25 }, XCSRRepr::kCSR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.75, 0.75 }, XCSRRepr::kCSR));
    EXPECT_FALSE(cond.matches({ 0.25, 0.25, 0.75, 0.75 }, XCSRRepr::kCSR));
    EXPECT_FALSE(cond.matches({ 0.75, 0.75, 0.75, 0.75 }, XCSRRepr::kCSR));
    EXPECT_FALSE(cond.matches({ 0.75, 0.25, 0.75, 0.75 }, XCSRRepr::kCSR));
}

TEST(XCSR_ConditionTest, IsMoreGeneral_CSR)
{
    const xcsr::Condition cond1("0.25;0.25 0.75;0.25 0.5;0.5 0.5;0.5");
    const xcsr::Condition cond2("0.25;0.25 0.75;0.25 0.5;0.5 0.75;0.25");
    const xcsr::Condition cond3("0.75;0.25 0.75;0.25 0.5;0.5 0.5;0.5");

    EXPECT_FALSE(cond1.isMoreGeneral(cond1, XCSRRepr::kCSR));
    EXPECT_FALSE(cond2.isMoreGeneral(cond2, XCSRRepr::kCSR));
    EXPECT_FALSE(cond3.isMoreGeneral(cond3, XCSRRepr::kCSR));

    // "01##" vs. "01#1"
    EXPECT_TRUE(cond1.isMoreGeneral(cond2, XCSRRepr::kCSR));
    EXPECT_FALSE(cond2.isMoreGeneral(cond1, XCSRRepr::kCSR));

    // "01##" vs. "11##"
    EXPECT_FALSE(cond1.isMoreGeneral(cond3, XCSRRepr::kCSR));
    EXPECT_FALSE(cond3.isMoreGeneral(cond1, XCSRRepr::kCSR));

    // "01#1" vs. "11##"
    EXPECT_FALSE(cond2.isMoreGeneral(cond3, XCSRRepr::kCSR));
    EXPECT_FALSE(cond3.isMoreGeneral(cond2, XCSRRepr::kCSR));

    // vs. "####"
    const xcsr::Condition allDontCare("0.5;0.5 0.6;0.6 0.3;0.7 0.8;0.8");
    EXPECT_TRUE(allDontCare.isMoreGeneral(cond1, XCSRRepr::kCSR));
    EXPECT_TRUE(allDontCare.isMoreGeneral(cond2, XCSRRepr::kCSR));
    EXPECT_TRUE(allDontCare.isMoreGeneral(cond3, XCSRRepr::kCSR));
    EXPECT_FALSE(cond1.isMoreGeneral(allDontCare, XCSRRepr::kCSR));
    EXPECT_FALSE(cond2.isMoreGeneral(allDontCare, XCSRRepr::kCSR));
    EXPECT_FALSE(cond3.isMoreGeneral(allDontCare, XCSRRepr::kCSR));
}

TEST(XCSR_ConditionTest, ConstructWithVector_OBR)
{
    const xcsr::Symbol zero(0.0, 0.5);
    const xcsr::Symbol one(0.5, 1.0);
    const xcsr::Symbol dontCare(0.0, 1.0);

    // "01##"
    const xcsr::Condition cond({ zero, one, dontCare, dontCare });
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.25, 0.25 }, XCSRRepr::kOBR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.25, 0.75 }, XCSRRepr::kOBR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.75, 0.25 }, XCSRRepr::kOBR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.75, 0.75 }, XCSRRepr::kOBR));
    EXPECT_FALSE(cond.matches({ 0.25, 0.25, 0.75, 0.75 }, XCSRRepr::kOBR));
    EXPECT_FALSE(cond.matches({ 0.75, 0.75, 0.75, 0.75 }, XCSRRepr::kOBR));
    EXPECT_FALSE(cond.matches({ 0.75, 0.25, 0.75, 0.75 }, XCSRRepr::kOBR));
}

TEST(XCSR_ConditionTest, ConstructWithString_OBR)
{
    // "01##"
    const xcsr::Condition cond("0.0;0.5 0.5;1.0 0.0;1.0 0.0;1.0");
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.25, 0.25 }, XCSRRepr::kOBR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.25, 0.75 }, XCSRRepr::kOBR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.75, 0.25 }, XCSRRepr::kOBR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.75, 0.75 }, XCSRRepr::kOBR));
    EXPECT_FALSE(cond.matches({ 0.25, 0.25, 0.75, 0.75 }, XCSRRepr::kOBR));
    EXPECT_FALSE(cond.matches({ 0.75, 0.75, 0.75, 0.75 }, XCSRRepr::kOBR));
    EXPECT_FALSE(cond.matches({ 0.75, 0.25, 0.75, 0.75 }, XCSRRepr::kOBR));
}


TEST(XCSR_ConditionTest, IsMoreGeneral_OBR)
{
    const xcsr::Condition cond1("0.0;0.5 0.5;1.0 0.0;1.0 0.0;1.0");
    const xcsr::Condition cond2("0.0;0.5 0.5;1.0 0.0;1.0 0.5;1.0");
    const xcsr::Condition cond3("0.5;1.0 0.5;1.0 0.0;1.0 0.0;1.0");

    EXPECT_FALSE(cond1.isMoreGeneral(cond1, XCSRRepr::kOBR));
    EXPECT_FALSE(cond2.isMoreGeneral(cond2, XCSRRepr::kOBR));
    EXPECT_FALSE(cond3.isMoreGeneral(cond3, XCSRRepr::kOBR));

    // "01##" vs. "01#1"
    EXPECT_TRUE(cond1.isMoreGeneral(cond2, XCSRRepr::kOBR));
    EXPECT_FALSE(cond2.isMoreGeneral(cond1, XCSRRepr::kOBR));

    // "01##" vs. "11##"
    EXPECT_FALSE(cond1.isMoreGeneral(cond3, XCSRRepr::kOBR));
    EXPECT_FALSE(cond3.isMoreGeneral(cond1, XCSRRepr::kOBR));

    // "01#1" vs. "11##"
    EXPECT_FALSE(cond2.isMoreGeneral(cond3, XCSRRepr::kOBR));
    EXPECT_FALSE(cond3.isMoreGeneral(cond2, XCSRRepr::kOBR));

    // vs. "####"
    const xcsr::Condition allDontCare("0.0;1.0 0.0;1.0 0.0;1.0 0.0;1.0");
    EXPECT_TRUE(allDontCare.isMoreGeneral(cond1, XCSRRepr::kOBR));
    EXPECT_TRUE(allDontCare.isMoreGeneral(cond2, XCSRRepr::kOBR));
    EXPECT_TRUE(allDontCare.isMoreGeneral(cond3, XCSRRepr::kOBR));
    EXPECT_FALSE(cond1.isMoreGeneral(allDontCare, XCSRRepr::kOBR));
    EXPECT_FALSE(cond2.isMoreGeneral(allDontCare, XCSRRepr::kOBR));
    EXPECT_FALSE(cond3.isMoreGeneral(allDontCare, XCSRRepr::kOBR));
}

TEST(XCSR_ConditionTest, ConstructWithVector_UBR)
{
    const xcsr::Symbol zero(0.0, 0.5);
    const xcsr::Symbol one(1.0, 0.5);
    const xcsr::Symbol dontCare(0.0, 1.0);

    // "01##"
    const xcsr::Condition cond({ zero, one, dontCare, dontCare });
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.25, 0.25 }, XCSRRepr::kUBR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.25, 0.75 }, XCSRRepr::kUBR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.75, 0.25 }, XCSRRepr::kUBR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.75, 0.75 }, XCSRRepr::kUBR));
    EXPECT_FALSE(cond.matches({ 0.25, 0.25, 0.75, 0.75 }, XCSRRepr::kUBR));
    EXPECT_FALSE(cond.matches({ 0.75, 0.75, 0.75, 0.75 }, XCSRRepr::kUBR));
    EXPECT_FALSE(cond.matches({ 0.75, 0.25, 0.75, 0.75 }, XCSRRepr::kUBR));
}

TEST(XCSR_ConditionTest, ConstructWithString_UBR)
{
    // "01##"
    const xcsr::Condition cond("0.5;0.0 0.5;1.0 1.0;0.0 0.0;1.0");
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.25, 0.25 }, XCSRRepr::kUBR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.25, 0.75 }, XCSRRepr::kUBR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.75, 0.25 }, XCSRRepr::kUBR));
    EXPECT_TRUE(cond.matches({ 0.25, 0.75, 0.75, 0.75 }, XCSRRepr::kUBR));
    EXPECT_FALSE(cond.matches({ 0.25, 0.25, 0.75, 0.75 }, XCSRRepr::kUBR));
    EXPECT_FALSE(cond.matches({ 0.75, 0.75, 0.75, 0.75 }, XCSRRepr::kUBR));
    EXPECT_FALSE(cond.matches({ 0.75, 0.25, 0.75, 0.75 }, XCSRRepr::kUBR));
}

TEST(XCSR_ConditionTest, IsMoreGeneral_UBR)
{
    const xcsr::Condition cond1("0.0;0.5 0.5;1.0 0.0;1.0 0.0;1.0");
    const xcsr::Condition cond2("0.0;0.5 0.5;1.0 0.0;1.0 0.5;1.0");
    const xcsr::Condition cond3("0.5;1.0 0.5;1.0 0.0;1.0 0.0;1.0");

    EXPECT_FALSE(cond1.isMoreGeneral(cond1, XCSRRepr::kUBR));
    EXPECT_FALSE(cond2.isMoreGeneral(cond2, XCSRRepr::kUBR));
    EXPECT_FALSE(cond3.isMoreGeneral(cond3, XCSRRepr::kUBR));

    // "01##" vs. "01#1"
    EXPECT_TRUE(cond1.isMoreGeneral(cond2, XCSRRepr::kUBR));
    EXPECT_FALSE(cond2.isMoreGeneral(cond1, XCSRRepr::kUBR));

    // "01##" vs. "11##"
    EXPECT_FALSE(cond1.isMoreGeneral(cond3, XCSRRepr::kUBR));
    EXPECT_FALSE(cond3.isMoreGeneral(cond1, XCSRRepr::kUBR));

    // "01#1" vs. "11##"
    EXPECT_FALSE(cond2.isMoreGeneral(cond3, XCSRRepr::kUBR));
    EXPECT_FALSE(cond3.isMoreGeneral(cond2, XCSRRepr::kUBR));

    // vs. "####"
    const xcsr::Condition allDontCare("1.0;0.0 0.0;1.0 1.0;0.0 0.0;1.0");
    EXPECT_TRUE(allDontCare.isMoreGeneral(cond1, XCSRRepr::kUBR));
    EXPECT_TRUE(allDontCare.isMoreGeneral(cond2, XCSRRepr::kUBR));
    EXPECT_TRUE(allDontCare.isMoreGeneral(cond3, XCSRRepr::kUBR));
    EXPECT_FALSE(cond1.isMoreGeneral(allDontCare, XCSRRepr::kUBR));
    EXPECT_FALSE(cond2.isMoreGeneral(allDontCare, XCSRRepr::kUBR));
    EXPECT_FALSE(cond3.isMoreGeneral(allDontCare, XCSRRepr::kUBR));
}
