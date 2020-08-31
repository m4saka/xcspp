#pragma once
#include <ostream>
#include <string>
#include <vector>
#include <cstddef> // std::size_t

#include "symbol.hpp"
#include "random.hpp"

namespace xcspp
{

    class Condition
    {
    protected:
        std::vector<Symbol> m_symbols;

    public:
        // Constructor
        Condition() = default;

        Condition(const std::vector<Symbol> & symbols) : m_symbols(symbols) {}

        Condition(const std::vector<int> & symbols) : m_symbols(symbols.begin(), symbols.end()) {}

        explicit Condition(const std::string & symbols)
        {
            for (const char symbol : symbols)
            {
                m_symbols.emplace_back(symbol);
            }
        }

        // Destructor
        ~Condition() = default;

        std::string toString() const
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

        Symbol & operator[] (std::size_t idx)
        {
            return m_symbols[idx];
        }

        const Symbol & operator[] (std::size_t idx) const
        {
            return m_symbols[idx];
        }

        Symbol & at(std::size_t idx)
        {
            return m_symbols.at(idx);
        }

        const Symbol & at(std::size_t idx) const
        {
            return m_symbols.at(idx);
        }

        friend std::ostream & operator<< (std::ostream & os, const Condition & obj)
        {
            return os << obj.toString();
        }

        friend bool operator== (const Condition & lhs, const Condition & rhs)
        {
            return lhs.m_symbols == rhs.m_symbols;
        }

        friend bool operator!= (const Condition & lhs, const Condition & rhs)
        {
            return lhs.m_symbols != rhs.m_symbols;
        }

        // DOES MATCH
        bool matches(const std::vector<int> & situation) const
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

        auto empty() const noexcept
        {
            return m_symbols.empty();
        }

        auto size() const noexcept
        {
            return m_symbols.size();
        }

        auto begin() const noexcept
        {
            return m_symbols.begin();
        }

        auto end() const noexcept
        {
            return m_symbols.end();
        }

        auto cbegin() const noexcept
        {
            return m_symbols.cbegin();
        }

        auto cend() const noexcept
        {
            return m_symbols.cend();
        }

        void setToDontCareAtRandom(double dontCareProbability)
        {
            for (auto & symbol : m_symbols)
            {
                if (Random::nextDouble() < dontCareProbability)
                {
                    symbol.setToDontCare();
                }
            }
        }

        std::size_t dontCareCount() const
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
    };

}
