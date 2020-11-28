#include "xcspp/core/xcsr/xcsr_repr.hpp"
#include <algorithm>

#include "xcspp/core/xcsr/symbol.hpp"

namespace xcspp::xcsr
{

    double GetLowerBound(const Symbol & s, XCSRRepr repr)
    {
        switch (repr)
        {
        case XCSRRepr::kCSR:
            return s.v1 - s.v2;

        case XCSRRepr::kOBR:
            return s.v1;

        case XCSRRepr::kUBR:
            return std::min(s.v1, s.v2);
        };

        return 0.0;
    }

    double GetUpperBound(const Symbol & s, XCSRRepr repr)
    {
        switch (repr)
        {
        case XCSRRepr::kCSR:
            return s.v1 + s.v2;

        case XCSRRepr::kOBR:
            return s.v2;

        case XCSRRepr::kUBR:
            return std::max(s.v1, s.v2);
        };

        return 0.0;
    }

    double ClampSymbolValue1(double v1, XCSRRepr repr, double minValue, double maxValue)
    {
        (void)repr; // Not used

        return std::clamp(v1, minValue, maxValue);
    }

    double ClampSymbolValue2(double v2, XCSRRepr repr, double minValue, double maxValue)
    {
        if (repr == XCSRRepr::kCSR)
        {
            return std::max(v2, 0.0);
        }
        else
        {
            return std::clamp(v2, minValue, maxValue);
        }
    }

}
