#pragma once
#include <cmath>
#include "xcspp/util/random.hpp"

namespace xcspp::xcsr
{

    // XCSR representation
    enum class XCSRRepr
    {
        kCSR, // Center-Spread Representation   [  c - s   ,  c + s   )
        kOBR, // Ordered-Bound Representation   [    l     ,    u     )
        kUBR, // Unordered-Bound Representation [ min(p,q) , max(p,q) )
    };

    // Forward declarations for function arguments
    class Symbol;
    struct XCSRParams;

    // --- Functions that depends on the representation ---

    double GetLowerBound(const Symbol & s, XCSRRepr repr);

    double GetUpperBound(const Symbol & s, XCSRRepr repr);

    double ClampSymbolValue1(double v1, XCSRRepr repr, double minValue, double maxValue);

    double ClampSymbolValue2(double v2, XCSRRepr repr, double minValue, double maxValue);

    Symbol MakeCoveringSymbol(double inputValue, const XCSRParams *pParams, Random & random);

}
