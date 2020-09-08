#include "xcspp/symbol.hpp"
#include <ostream>
#include <string>
#include <vector>
#include <stdexcept>

namespace xcspp
{

    Symbol::Symbol()
        : m_value(0)
        , m_isDontCare(true)
    {
    }

    Symbol::Symbol(int value)
        : m_value(value)
        , m_isDontCare(false)
    {
    }

    Symbol::Symbol(char c)
        : m_value((c == '#') ? 0 : static_cast<int>(c - '0'))
        , m_isDontCare(c == '#')
    {
        if (!m_isDontCare && (m_value < 0 || m_value >= 10))
        {
            throw std::invalid_argument(
                "Symbol::Symbol(char) received an invalid character.\n"
                "You need to use an integer ('0'-'9') or Don't Care symbol ('#').");
        }
    }

    std::string Symbol::toString() const
    {
        if (isDontCare())
            return "#";
        else
            return std::to_string(m_value);
    }

    // DOES MATCH
    bool Symbol::matches(int value) const
    {
        return isDontCare() || this->value() == value;
    }

    int Symbol::value() const
    {
        if (m_isDontCare)
        {
            throw std::domain_error(
                "Symbol::value() is called although the symbol is '#' (Don't Care).\n"
                "You need to confirm 'isDontCare() == false' before calling Symbol::value().");
        }
        return m_value;
    }

    void Symbol::setValue(int value)
    {
        m_value = value;
        m_isDontCare = false;
    }

    bool Symbol::isDontCare() const
    {
        return m_isDontCare;
    }

    void Symbol::setToDontCare()
    {
        m_isDontCare = true;
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
