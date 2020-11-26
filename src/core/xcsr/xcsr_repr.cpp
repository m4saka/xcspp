#include "xcspp/core/xcsr/xcsr_repr.hpp"
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

}
