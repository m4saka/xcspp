#include <gtest/gtest.h>
#include <xcspp/xcspp.hpp>

using namespace xcspp;

TEST(XCS_SymbolTest, ConstructWithNumberChar)
{
    // Numbers
    for (int i = 0; i < 10; ++i)
    {
        // Generate '0'--'9' character
        const char c = '0' + i;

        const xcs::Symbol number(c);
        EXPECT_FALSE(number.isDontCare());
        EXPECT_EQ(number.value(), i);
        EXPECT_TRUE(number.matches(i));
        EXPECT_FALSE(number.matches(i + 1));
    }
}

TEST(XCS_SymbolTest, ConstructWithDontCareChar)
{
    // Don't Care
    const xcs::Symbol dontCare('#');
    EXPECT_TRUE(dontCare.isDontCare());
    EXPECT_TRUE(dontCare.matches(0));
    EXPECT_TRUE(dontCare.matches(1));
}

TEST(XCS_SymbolTest, ConstructWithInt)
{
    for (int i = 0; i < 10; ++i)
    {
        const xcs::Symbol number(i);
        EXPECT_FALSE(number.isDontCare());
        EXPECT_EQ(number.value(), i);
        EXPECT_TRUE(number.matches(i));
        EXPECT_FALSE(number.matches(i + 1));
    }
}

TEST(XCS_SymbolTest, CopyConstruct)
{
    const xcs::Symbol one(1);
    const xcs::Symbol oneClone(one);
    EXPECT_FALSE(oneClone.isDontCare());
    EXPECT_EQ(oneClone.value(), 1);
    EXPECT_TRUE(oneClone.matches(1));
    EXPECT_FALSE(oneClone.matches(0));
    EXPECT_EQ(one, oneClone);

    const xcs::Symbol dontCare('#');
    const xcs::Symbol dontCareClone(dontCare);
    EXPECT_TRUE(dontCareClone.isDontCare());
    EXPECT_TRUE(dontCareClone.matches(0));
    EXPECT_TRUE(dontCareClone.matches(1));
    EXPECT_EQ(dontCare, dontCareClone);
}

TEST(XCS_SymbolTest, SetValue)
{
    xcs::Symbol symbol(1);
    EXPECT_FALSE(symbol.isDontCare());
    EXPECT_EQ(symbol.value(), 1);
    EXPECT_TRUE(symbol.matches(1));
    EXPECT_FALSE(symbol.matches(0));

    symbol.setValue(0);
    EXPECT_FALSE(symbol.isDontCare());
    EXPECT_EQ(symbol.value(), 0);
    EXPECT_TRUE(symbol.matches(0));
    EXPECT_FALSE(symbol.matches(1));

    symbol.setToDontCare();
    EXPECT_TRUE(symbol.isDontCare());
    EXPECT_TRUE(symbol.matches(0));
    EXPECT_TRUE(symbol.matches(1));

    symbol.setValue(1);
    EXPECT_FALSE(symbol.isDontCare());
    EXPECT_EQ(symbol.value(), 1);
    EXPECT_TRUE(symbol.matches(1));
    EXPECT_FALSE(symbol.matches(0));
}

TEST(XCS_SymbolTest, AssignmentAndComparison)
{
    xcs::Symbol symbol(0);
    xcs::Symbol zero = symbol;

    symbol.setToDontCare();
    xcs::Symbol dontCare = symbol;

    symbol.setValue(1);
    xcs::Symbol one = symbol;

    symbol.setValue(0);
    EXPECT_EQ(symbol, zero);
    EXPECT_NE(symbol, dontCare);
    EXPECT_NE(symbol, one);
    EXPECT_NE(zero, dontCare);
    EXPECT_NE(zero, one);
    EXPECT_NE(dontCare, one);
}
