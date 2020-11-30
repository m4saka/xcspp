#include "xcspp/core/xcsr/xcsr_repr.hpp"
#include <algorithm>

#include "xcspp/core/xcsr/symbol.hpp"
#include "xcspp/core/xcsr/xcsr_params.hpp"

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

    Symbol MakeCoveringSymbol(double inputValue, const XCSRParams *pParams, Random & random)
    {
        double v1;
        double v2;

        switch (pParams->repr)
        {
        case XCSRRepr::kCSR:
            v1 = inputValue; // Center
            v2 = random.nextDouble(0.0, pParams->s0); // Spread
            break;

        case XCSRRepr::kOBR:
        case XCSRRepr::kUBR:
            {
                double lowerMin = inputValue - pParams->s0;
                double upperMax = inputValue + pParams->s0;
                if (pParams->doCoveringRandomRangeTruncation)
                {
                    lowerMin = std::max(lowerMin, pParams->minValue);
                    upperMax = std::min(upperMax, pParams->maxValue);
                }

                v1 = random.nextDouble(lowerMin, inputValue);
                v2 = random.nextDouble(inputValue, upperMax);
                if (pParams->doRangeRestriction)
                {
                    v1 = std::max(v1, pParams->minValue);
                    v2 = std::min(v2, pParams->maxValue);
                }

                if (pParams->repr == XCSRRepr::kUBR && random.nextDouble() < 0.5)
                {
                    std::swap(v1, v2);
                }
            }
            break;
        
        default:
            // Zero assignment for avoiding maybe-uninitialized warning
            v1 = 0.0;
            v2 = 0.0;
        };

        return Symbol(v1, v2);
    }

}
