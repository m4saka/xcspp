#include "xcspp/environment/even_parity_environment.hpp"
#include <stdexcept>

namespace xcspp
{

    namespace
    {
        void SetRandomSituation(std::vector<int> & situation, Random & random)
        {
            for (auto & s : situation)
            {
                s = random.nextInt(0, 1);
            }
        }

        int GetAnswerOfSituation(const std::vector<int> & situation)
        {
            int sum = 0;
            for (int s : situation)
            {
                sum += s; // s is 0 or 1 here
            }
            return (sum % 2) == 0; // even => 1, odd => 0
        }
    }

    EvenParityEnvironment::EvenParityEnvironment(std::size_t length)
        : m_length(length)
        , m_situation(length)
        , m_isEndOfProblem(false)
    {
        SetRandomSituation(m_situation, m_random);
    }

    std::vector<int> EvenParityEnvironment::situation() const
    {
        return m_situation;
    }

    double EvenParityEnvironment::executeAction(int action)
    {
        double reward = (action == getAnswer()) ? 1000.0 : 0.0;

        // Update situation
        SetRandomSituation(m_situation, m_random);

        // Single-step problem
        m_isEndOfProblem = true;

        return reward;
    }

    bool EvenParityEnvironment::isEndOfProblem() const
    {
        return m_isEndOfProblem;
    }

    int EvenParityEnvironment::getAnswer() const
    {
        return GetAnswerOfSituation(m_situation);
    }

}
