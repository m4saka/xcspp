#pragma once
#include <ostream>
#include <string>
#include <vector>
#include <cstddef> // std::size_t

#include "symbol.hpp"

namespace xcspp
{

    class Condition
    {
    protected:
        std::vector<Symbol> m_symbols;

    public:
        // Constructor
        Condition() = default;

        Condition(const std::vector<Symbol> & symbols);

        Condition(const std::vector<int> & symbols);

        explicit Condition(const std::string & symbols);

        // Destructor
        ~Condition() = default;

        std::string toString() const;

        // DOES MATCH
        bool matches(const std::vector<int> & situation) const;

        void setToDontCareAtRandom(double dontCareProbability);

        std::size_t dontCareCount() const;

        friend std::ostream & operator<< (std::ostream & os, const Condition & obj);

        // --- The functions below are just the wrapper for std::vector<Symbol> ---

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

        friend bool operator== (const Condition & lhs, const Condition & rhs)
        {
            return lhs.m_symbols == rhs.m_symbols;
        }

        friend bool operator!= (const Condition & lhs, const Condition & rhs)
        {
            return lhs.m_symbols != rhs.m_symbols;
        }
    };

}
