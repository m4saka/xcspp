#pragma once
#include <ostream> // operator<<
#include <string>
#include <vector>
#include <cstddef> // std::size_t

#include "symbol.hpp"

namespace xcspp::xcsr
{

    class Condition
    {
    private:
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

        // IS MORE GENERAL
        bool isMoreGeneral(const Condition & cl) const;

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

        auto begin() noexcept
        {
            return m_symbols.begin();
        }

        auto begin() const noexcept
        {
            return m_symbols.begin();
        }

        auto end() noexcept
        {
            return m_symbols.end();
        }

        auto end() const noexcept
        {
            return m_symbols.end();
        }

        auto rbegin() noexcept
        {
            return m_symbols.rbegin();
        }

        auto rbegin() const noexcept
        {
            return m_symbols.rbegin();
        }

        auto rend() noexcept
        {
            return m_symbols.rend();
        }

        auto rend() const noexcept
        {
            return m_symbols.rend();
        }

        auto cbegin() const noexcept
        {
            return m_symbols.cbegin();
        }

        auto cend() const noexcept
        {
            return m_symbols.cend();
        }

        auto crbegin() const noexcept
        {
            return m_symbols.crbegin();
        }

        auto crend() const noexcept
        {
            return m_symbols.crend();
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
