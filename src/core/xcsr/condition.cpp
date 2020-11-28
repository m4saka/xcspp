#include "xcspp/core/xcsr/condition.hpp"
#include <sstream>

#include "xcspp/util/random.hpp"

namespace xcspp::xcsr
{

    Condition::Condition(const std::vector<Symbol> & symbols) : m_symbols(symbols) {}

    Condition::Condition(const std::string & symbols)
    {
        std::istringstream iss(symbols);
        std::string symbol;
        while (std::getline(iss, symbol, ' '))
        {
            if (symbol.empty())
            {
                continue;
            }

            m_symbols.emplace_back(symbol);
        }
    }

    std::string Condition::toString() const
    {
        std::string str;
        str.reserve(m_symbols.size() * 7); // 'x.x;y.y': 7 chars (minimum length)
        for (const auto & symbol : m_symbols)
        {
            str += symbol.toString();
            str += ' ';
        }

        // Erase last whitespace
        if (!str.empty() && str.back() == ' ')
        {
            str.pop_back();
        }

        return str;
    }

    // DOES MATCH
    bool Condition::matches(const std::vector<double> & situation, XCSRRepr repr) const
    {
        if (m_symbols.size() != situation.size())
        {
            std::invalid_argument("Condition::matches() could not process the situation with a different length.");
        }

        for (std::size_t i = 0; i < m_symbols.size(); ++i)
        {
            if (!m_symbols[i].matches(situation[i], repr))
            {
                return false;
            }
        }

        return true;
    }

    bool Condition::isMoreGeneral(const Condition & cond, XCSRRepr repr) const
    {
        if (m_symbols.size() != cond.size())
        {
            throw std::invalid_argument("In Condition::isMoreGeneral(), both conditions must have the same length.");
        }

        std::size_t equalCount = 0;

        for (std::size_t i = 0; i < m_symbols.size(); ++i)
        {
            const double otherL = GetLowerBound(cond[i], repr);
            const double otherU = GetUpperBound(cond[i], repr);
            const double selfL = GetLowerBound(m_symbols[i], repr);
            const double selfU = GetUpperBound(m_symbols[i], repr);

            if (otherL < selfL || selfU < otherU)
            {
                return false;
            }

            if (otherL == selfL && selfU == otherU)
            {
                ++equalCount;
            }
        }

        if (equalCount == m_symbols.size())
        {
            return false;
        }

        return true;
    }

    std::ostream & operator<< (std::ostream & os, const Condition & obj)
    {
        return os << obj.toString();
    }

}
