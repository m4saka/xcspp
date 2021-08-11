#include "xcspp/experiment/xcs/condition.hpp"
#include <sstream>

#include "xcspp/util/random.hpp"

namespace xcspp::xcs
{

    Condition::Condition(const std::vector<Symbol> & symbols) : m_symbols(symbols) {}

    Condition::Condition(const std::vector<int> & symbols) : m_symbols(symbols.begin(), symbols.end()) {}

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
        str.reserve(m_symbols.size() * 2);
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
    bool Condition::matches(const std::vector<int> & situation) const
    {
        if (m_symbols.size() != situation.size())
        {
            std::invalid_argument("Condition::matches() could not process the situation with a different length.");
        }

        for (std::size_t i = 0; i < m_symbols.size(); ++i)
        {
            if (!m_symbols[i].matches(situation[i]))
            {
                return false;
            }
        }

        return true;
    }

    bool Condition::isMoreGeneral(const Condition & cond) const
    {
        if (m_symbols.size() != cond.size())
        {
            throw std::invalid_argument("In Condition::isMoreGeneral(), both conditions must have the same length.");
        }

        bool ret = false;

        for (std::size_t i = 0; i < m_symbols.size(); ++i)
        {
            if (m_symbols[i] != cond[i])
            {
                if (m_symbols[i].isDontCare())
                {
                    ret = true;
                }
                else 
                {
                    return false;
                }
            }
        }

        return ret;
    }

    std::size_t Condition::dontCareCount() const
    {
        std::size_t count = 0;
        for (const auto & symbol : m_symbols)
        {
            if (symbol.isDontCare())
            {
                count++;
            }
        }

        return count;
    }

    std::ostream & operator<< (std::ostream & os, const Condition & obj)
    {
        return os << obj.toString();
    }

}
