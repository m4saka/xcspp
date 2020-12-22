#include "xcspp/core/xcsr/symbol.hpp"
#include <string>
#include <vector>
#include <stdexcept>

namespace xcspp::xcsr
{

    Symbol::Symbol(double v1, double v2)
        : v1(v1)
        , v2(v2)
    {
    }

    Symbol::Symbol(const std::string & str)
    {
        const std::size_t semicolonIdx = str.find(';');

        // Make sure the string has at least one semicolon
        if (semicolonIdx == std::string::npos)
        {
            throw std::invalid_argument("Could not construct XCSR Symbol from string because it does not contain ';' separator.");
        }

        v1 = std::stod(str.substr(0, semicolonIdx));
        v2 = std::stod(str.substr(semicolonIdx + 1));
    }

    std::string Symbol::toString() const
    {
        return std::to_string(v1) + ";" + std::to_string(v2);
    }

    // DOES MATCH
    bool Symbol::matches(double value, XCSRRepr repr) const
    {
        return GetLowerBound(*this, repr) <= value && value < GetUpperBound(*this, repr);
    }

    std::ostream & operator<< (std::ostream & os, const Symbol & obj)
    {
        return os << obj.toString();
    }

    bool operator== (const Symbol & lhs, const Symbol & rhs)
    {
        return lhs.v1 == rhs.v1 && lhs.v2 == rhs.v2;
    }

    bool operator!= (const Symbol & lhs, const Symbol & rhs)
    {
        return !(lhs == rhs);
    }

}
