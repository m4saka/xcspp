#pragma once
#include <ostream>
#include <string>
#include <vector>
#include <stdexcept>

namespace xcscpp
{

    // The standard symbol for XCS (integer or "#" = "Don't Care")
    class Symbol
    {
    protected:
        int m_value;
        bool m_isDontCare;

    public:
        // Constructor (default)
        constexpr Symbol()
            : m_value(0)
            , m_isDontCare(true)
        {
        }

        // Constructor (with integer)
        constexpr explicit Symbol(int value)
            : m_value(value)
            , m_isDontCare(false)
        {
        }

        // Constructor (with character: '0'-'9' or '#')
        constexpr explicit Symbol(char c)
            : m_value((c == '#') ? 0 : static_cast<int>(c - '0'))
            , m_isDontCare(c == '#')
        {
        }

        // Constructor (copy)
        constexpr Symbol(const Symbol & obj) = default;

        // Destructor
        ~Symbol() = default;

        std::string toString() const
        {
            if (isDontCare())
                return "#";
            else
                return std::to_string(m_value);
        }

        friend std::ostream & operator<< (std::ostream & os, const Symbol & obj)
        {
            return os << obj.toString();
        }

        friend bool operator== (const Symbol & lhs, const Symbol & rhs)
        {
            return lhs.isDontCare() == rhs.isDontCare() && (lhs.isDontCare() || lhs.value() == rhs.value());
        }

        friend bool operator!= (const Symbol & lhs, const Symbol & rhs)
        {
            return !(lhs == rhs);
        }

        Symbol & operator= (const Symbol & obj)
        {
            m_value = obj.m_value;
            m_isDontCare = obj.m_isDontCare;
            return *this;
        }

        // DOES MATCH
        bool matches(int value) const
        {
            return isDontCare() || this->value() == value;
        }

        // Returns integer value
        // (make sure to confirm "isDontCare() == false" before calling this)
        int value() const
        {
            if (m_isDontCare)
            {
                throw std::domain_error(
                    "Symbol::value() is called although the symbol is '#' (Don't Care).\n"
                    "You need to confirm 'isDontCare() == false' before calling Symbol::value().");
            }
            return m_value;
        }

        // Set to integer value
        void setValue(int value)
        {
            m_value = value;
            m_isDontCare = false;
        }

        // Returns whether the symbol is "#" (Don't Care) or not
        bool isDontCare() const
        {
            return m_isDontCare;
        }

        void setToDontCare()
        {
            m_isDontCare = true;
        }
    };

}
