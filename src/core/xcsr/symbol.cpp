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
        // FIXME: Not implemented yet
        exit(1);
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
        return lhs.isDontCare() == rhs.isDontCare() && (lhs.isDontCare() || lhs.value() == rhs.value());
    }

    bool operator!= (const Symbol & lhs, const Symbol & rhs)
    {
        return !(lhs == rhs);
    }

}
