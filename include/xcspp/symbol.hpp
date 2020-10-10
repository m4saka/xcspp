#pragma once
#include <ostream>
#include <string>
#include <vector>
#include <stdexcept>

namespace xcspp
{

    // The standard symbol for XCS (integer or "#" = "Don't Care")
    class Symbol
    {
    private:
        int m_value;
        bool m_isDontCare;

    public:
        // Constructor (default)
        Symbol();

        // Constructor (with integer)
        explicit Symbol(int value);

        // Constructor (with character '0'-'9' or '#')
        explicit Symbol(char c);

        // Constructor (with integer string or "#")
        explicit Symbol(const std::string & str);

        // Constructor (copy)
        Symbol(const Symbol &) = default;

        // Assignment operator
        Symbol & operator= (const Symbol & obj) = default;

        // Destructor
        ~Symbol() = default;

        std::string toString() const;

        // DOES MATCH
        bool matches(int value) const;

        // Returns integer value
        // (make sure to confirm "isDontCare() == false" before calling this)
        int value() const;

        // Set to integer value
        void setValue(int value);

        // Returns whether the symbol is "#" (Don't Care) or not
        bool isDontCare() const;

        void setToDontCare();

        friend std::ostream & operator<< (std::ostream & os, const Symbol & obj);

        friend bool operator== (const Symbol & lhs, const Symbol & rhs);

        friend bool operator!= (const Symbol & lhs, const Symbol & rhs);
    };

}
