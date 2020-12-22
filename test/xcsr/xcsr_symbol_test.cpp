#include <gtest/gtest.h>
#include <xcspp/xcspp.hpp>

using namespace xcspp;

TEST(XCSR_SymbolTest, ConstructWithString)
{
    const xcsr::Symbol symbol("0.25;0.5");

    // CSR
    EXPECT_TRUE(symbol.matches(0.0, xcsr::XCSRRepr::kCSR));
    EXPECT_TRUE(symbol.matches(0.3, xcsr::XCSRRepr::kCSR));
    EXPECT_FALSE(symbol.matches(-0.3, xcsr::XCSRRepr::kCSR));
    EXPECT_FALSE(symbol.matches(0.8, xcsr::XCSRRepr::kCSR));
    EXPECT_FALSE(symbol.matches(1.0, xcsr::XCSRRepr::kCSR));

    // OBR
    EXPECT_TRUE(symbol.matches(0.3, xcsr::XCSRRepr::kOBR));
    EXPECT_TRUE(symbol.matches(0.4, xcsr::XCSRRepr::kOBR));
    EXPECT_FALSE(symbol.matches(0.0, xcsr::XCSRRepr::kOBR));
    EXPECT_FALSE(symbol.matches(0.2, xcsr::XCSRRepr::kOBR));
    EXPECT_FALSE(symbol.matches(0.6, xcsr::XCSRRepr::kOBR));
    EXPECT_FALSE(symbol.matches(1.0, xcsr::XCSRRepr::kOBR));

    // UBR
    EXPECT_TRUE(symbol.matches(0.3, xcsr::XCSRRepr::kUBR));
    EXPECT_TRUE(symbol.matches(0.4, xcsr::XCSRRepr::kUBR));
    EXPECT_FALSE(symbol.matches(0.0, xcsr::XCSRRepr::kUBR));
    EXPECT_FALSE(symbol.matches(0.2, xcsr::XCSRRepr::kUBR));
    EXPECT_FALSE(symbol.matches(0.6, xcsr::XCSRRepr::kUBR));
    EXPECT_FALSE(symbol.matches(1.0, xcsr::XCSRRepr::kUBR));
}

TEST(XCSR_SymbolTest, CopyConstruct)
{
    const xcsr::Symbol symbol(0.25, 0.5);
    const xcsr::Symbol symbolClone(symbol);
    EXPECT_DOUBLE_EQ(symbolClone.v1, 0.25);
    EXPECT_DOUBLE_EQ(symbolClone.v2, 0.5);
    EXPECT_TRUE(symbolClone.matches(0.5, xcsr::XCSRRepr::kCSR));
    EXPECT_FALSE(symbolClone.matches(0.8, xcsr::XCSRRepr::kCSR));
    EXPECT_TRUE(symbolClone.matches(0.3, xcsr::XCSRRepr::kOBR));
    EXPECT_FALSE(symbolClone.matches(0.6, xcsr::XCSRRepr::kOBR));
    EXPECT_TRUE(symbolClone.matches(0.3, xcsr::XCSRRepr::kUBR));
    EXPECT_FALSE(symbolClone.matches(0.6, xcsr::XCSRRepr::kUBR));
    EXPECT_EQ(symbol, symbolClone);
}

TEST(XCSR_SymbolTest, LowerBound)
{
    xcsr::Symbol symbol1(0.5, 0.25);
    EXPECT_DOUBLE_EQ(xcsr::GetLowerBound(symbol1, xcsr::XCSRRepr::kCSR), 0.5 - 0.25);
    EXPECT_DOUBLE_EQ(xcsr::GetLowerBound(symbol1, xcsr::XCSRRepr::kUBR), 0.25);

    xcsr::Symbol symbol2(0.3, 0.6);
    EXPECT_DOUBLE_EQ(xcsr::GetLowerBound(symbol2, xcsr::XCSRRepr::kCSR), 0.3 - 0.6);
    EXPECT_DOUBLE_EQ(xcsr::GetLowerBound(symbol2, xcsr::XCSRRepr::kOBR), 0.3);
    EXPECT_DOUBLE_EQ(xcsr::GetLowerBound(symbol2, xcsr::XCSRRepr::kUBR), 0.3);
}

TEST(XCSR_SymbolTest, UpperBound)
{
    xcsr::Symbol symbol1(0.5, 0.25);
    EXPECT_DOUBLE_EQ(xcsr::GetUpperBound(symbol1, xcsr::XCSRRepr::kCSR), 0.5 + 0.25);
    EXPECT_DOUBLE_EQ(xcsr::GetUpperBound(symbol1, xcsr::XCSRRepr::kUBR), 0.5);

    xcsr::Symbol symbol2(0.3, 0.6);
    EXPECT_DOUBLE_EQ(xcsr::GetUpperBound(symbol2, xcsr::XCSRRepr::kCSR), 0.3 + 0.6);
    EXPECT_DOUBLE_EQ(xcsr::GetUpperBound(symbol2, xcsr::XCSRRepr::kOBR), 0.6);
    EXPECT_DOUBLE_EQ(xcsr::GetUpperBound(symbol2, xcsr::XCSRRepr::kUBR), 0.6);
}
