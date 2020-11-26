#pragma once
#include <cmath>

namespace xcspp::xcsr
{

    // XCSR representation
    enum class XCSRRepr
    {
        kCSR, // Center-Spread Representation   [  c - s   ,  c + s   )
        kOBR, // Ordered-Bound Representation   [    l     ,    u     )
        kUBR, // Unordered-Bound Representation [ min(p,q) , max(p,q) )
    };

    class Symbol;

    double GetLowerBound(const Symbol & s, XCSRRepr repr);

    double GetUpperBound(const Symbol & s, XCSRRepr repr);

}
