#pragma once

#include <vector>
#include <cstddef> // std::size_t

#include "ienvironment.hpp"

namespace xcspp
{

    class MultiplexerEnvironment : public IEnvironment
    {
    private:
        const std::size_t m_totalLength;
        const std::size_t m_addressBitLength;
        const double m_minorityAcceptanceProbability;
        std::vector<int> m_situation;
        bool m_isEndOfProblem;

    public:
        // Constructor
        // (Set imbalanceLevel to higher than 0 for imbalanced multiplexer problems)
        explicit MultiplexerEnvironment(std::size_t length, unsigned int imbalanceLevel = 0);

        virtual ~MultiplexerEnvironment() = default;

        virtual std::vector<int> situation() const override;

        virtual double executeAction(int action) override;

        virtual bool isEndOfProblem() const override;

        virtual std::unordered_set<int> availableActions() const override
        {
            return { 0, 1 };
        }

        // Returns answer to situation
        int getAnswer() const;
    };

}
