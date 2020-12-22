#pragma once
#include <vector>
#include <cstddef> // std::size_t

#include "ienvironment.hpp"
#include "xcspp/util/random.hpp"

namespace xcspp
{

    class RealMultiplexerEnvironment : public IRealEnvironment
    {
    private:
        const double m_minorityAcceptanceProbability;
        const double m_binaryThreshold;
        std::vector<double> m_situation;
        bool m_isEndOfProblem;
        Random m_random;

    public:
        // Constructor
        // (Set imbalanceLevel to higher than 0 for imbalanced multiplexer problems)
        explicit RealMultiplexerEnvironment(std::size_t length, unsigned int imbalanceLevel = 0, double binaryThreshold = 0.5);

        // Destructor
        virtual ~RealMultiplexerEnvironment() = default;

        // Returns current situation
        virtual std::vector<double> situation() const override;

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
