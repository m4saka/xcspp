#pragma once
#include <ostream> // operator<<
#include <string>
#include <vector>
#include <stdexcept>

#include "xcsr_repr.hpp"

namespace xcspp::xcsr
{

    // The symbol for XCSR
    class Symbol
    {
    public:
        // 1st attribute (e.g., c for CSR)
        double v1;

        // 2nd attribute (e.g., s for CSR)
        double v2;

        // Constructor (default)
        Symbol() = default;

        // Constructor (with integer)
        Symbol(double v1, double v2);

        // Constructor (with ';'-separated string; e.g., '0.2;0.1')
        explicit Symbol(const std::string & str);

        // Constructor (copy)
        Symbol(const Symbol &) = default;

        // Assignment operator
        Symbol & operator= (const Symbol & obj) = default;

        // Destructor
        ~Symbol() = default;

        std::string toString() const;

        // DOES MATCH
        bool matches(double value, XCSRRepr repr) const;

        friend std::ostream & operator<< (std::ostream & os, const Symbol & obj);

        friend bool operator== (const Symbol & lhs, const Symbol & rhs);

        friend bool operator!= (const Symbol & lhs, const Symbol & rhs);
    };

}
