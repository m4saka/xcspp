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

        // Destructor
        virtual ~MultiplexerEnvironment() = default;

        // Returns current situation
        virtual std::vector<int> situation() const override;

        // Executes action (and update situation), and returns reward
        virtual double executeAction(int action) override;

        // Returns true if the problem was solved by the previous action
        // (Since it is a single-step problem, this function always returns true after the first action execution)
        virtual bool isEndOfProblem() const override;

        // Returns available action choices
        virtual std::unordered_set<int> availableActions() const override
        {
            return { 0, 1 };
        }

        // Returns answer to situation
        int getAnswer() const;
    };

}
