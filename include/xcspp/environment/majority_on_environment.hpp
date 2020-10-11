#pragma once
#include <vector>
#include <cstddef> // std::size_t
#include <stdexcept>

#include "ienvironment.hpp"
#include "xcspp/random.hpp"

namespace xcspp
{

    class MajorityOnEnvironment : public IEnvironment
    {
    private:
        const std::size_t m_length;
        std::vector<bool> m_situation;
        bool m_isEndOfProblem;
        Random m_random;

        static std::vector<bool> randomSituation(std::size_t length)
        {
            std::vector<bool> situation;
            situation.reserve(length);
            for (std::size_t i = 0; i < length; ++i)
            {
                situation.push_back(Random::nextInt(0, 1));
            }
            return situation;
        }

    public:
        // Constructor
        explicit MajorityOnEnvironment(std::size_t length)
            : m_length(length)
            , m_situation(randomSituation(length))
            , m_isEndOfProblem(false)
        {
            if ((m_length % 2) == 0)
            {
                throw std::invalid_argument("The length parameter of MajorityOnEnvironment must be an odd number.");
            }
        }

        // Destructor
        virtual ~MajorityOnEnvironment() = default;

        // Returns current situation
        virtual std::vector<bool> situation() const override
        {
            return m_situation;
        }

        // Executes action (and update situation), and returns reward
        virtual double executeAction(bool action) override
        {
            double reward = (action == getAnswer()) ? 1000.0 : 0.0;

            // Update situation
            m_situation = randomSituation(m_length);

            // Single-step problem
            m_isEndOfProblem = true;

            return reward;
        }

        // Returns true if the problem was solved by the previous action
        // (Since it is a single-step problem, this function always returns true after the first action execution)
        virtual bool isEndOfProblem() const override
        {
            return m_isEndOfProblem;
        }

        // Returns answer to situation
        bool getAnswer() const
        {
            std::size_t sum = 0;
            for (bool b : m_situation)
            {
                sum += static_cast<std::size_t>(b);
            }
            return sum > m_length / 2;
        }

        // Returns available action choices (e.g. { 0, 1 })
        virtual std::unordered_set<int> availableActions() const override
        {
            return { 0, 1 };
        }
    };

}
