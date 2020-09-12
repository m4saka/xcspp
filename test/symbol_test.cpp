#include <gtest/gtest.h>
#include <xcspp.hpp>

TEST(SymbolTest, ConstructWithNumberChar)
{
    // Numbers
    for (int i = 0; i < 10; ++i)
    {
        // Generate '0'--'9' character
        const char c = '0' + i;

        const xcspp::Symbol number(c);
        EXPECT_FALSE(number.isDontCare());
        EXPECT_TRUE(number.value() == i);
        EXPECT_TRUE(number.matches(i));
        EXPECT_FALSE(number.matches(i + 1));
    }
}

TEST(SymbolTest, ConstructWithDontCareChar)
{
    // Don't Care
    const xcspp::Symbol dontCare('#');
    EXPECT_TRUE(dontCare.isDontCare());
    EXPECT_TRUE(dontCare.matches(0));
    EXPECT_TRUE(dontCare.matches(1));
}
