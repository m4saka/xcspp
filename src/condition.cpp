#include "xcspp/condition.hpp"

#include "xcspp/random.hpp"

namespace xcspp
{

    Condition::Condition(const std::vector<Symbol> & symbols) : m_symbols(symbols) {}

    Condition::Condition(const std::vector<int> & symbols) : m_symbols(symbols.begin(), symbols.end()) {}

    Condition::Condition(const std::string & symbols)
    {
        for (const char symbol : symbols)
        {
            m_symbols.emplace_back(symbol);
        }
    }

    std::string Condition::toString() const
    {
        std::string str;
        for (const auto & symbol : m_symbols)
        {
            str += symbol.toString();
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

    void Condition::setToDontCareAtRandom(double dontCareProbability)
    {
        for (auto & symbol : m_symbols)
        {
            if (Random::nextDouble() < dontCareProbability)
            {
                symbol.setToDontCare();
            }
        }
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
