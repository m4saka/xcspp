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

    double ClampSymbolValue1(double v1, XCSRRepr repr, double minValue, double maxValue, bool doRangeRestriction)
    {
        if (repr == XCSRRepr::kCSR)
        {
            // CSR
            return std::clamp(v1, minValue, maxValue);
        }
        else
        {
            // OBR and UBR
            if (doRangeRestriction)
            {
                return std::clamp(v1, minValue, maxValue);
            }
            else
            {
                return v1;
            }
        }
    }

    double ClampSymbolValue2(double v2, XCSRRepr repr, double minValue, double maxValue, bool doRangeRestriction)
    {
        if (repr == XCSRRepr::kCSR)
        {
            // CSR
            return std::max(v2, 0.0);
        }
        else
        {
            // OBR and UBR
            if (doRangeRestriction)
            {
                return std::clamp(v2, minValue, maxValue);
            }
            else
            {
                return v2;
            }
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

                if (pParams->repr == XCSRRepr::kUBR && random.nextDouble() < 0.5)
                {
                    std::swap(v1, v2);
                }

                v1 = ClampSymbolValue1(v1, pParams->repr, pParams->minValue, pParams->maxValue, pParams->doRangeRestriction);
                v2 = ClampSymbolValue2(v2, pParams->repr, pParams->minValue, pParams->maxValue, pParams->doRangeRestriction);
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
