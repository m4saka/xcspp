#include "xcspp/environment/majority_on_environment.hpp"
#include <stdexcept>

namespace xcspp
{

    namespace
    {
        void SetRandomSituation(std::vector<int> & situation, Random & random)
        {
            for (auto & s: situation)
            {
                s = random.nextInt(0, 1);
            }
        }

        int GetAnswerOfSituation(const std::vector<int> & situation)
        {
            std::size_t sum = 0;
            for (const auto & s : situation)
            {
                sum += static_cast<std::size_t>(s);
            }
            return (sum > situation.size() / 2) ? 1 : 0;
        }
    }

    MajorityOnEnvironment::MajorityOnEnvironment(std::size_t length)
        : m_length(length)
        , m_situation(length)
        , m_isEndOfProblem(false)
    {
        if ((m_length % 2) == 0)
        {
            throw std::invalid_argument("The length parameter of MajorityOnEnvironment must be an odd number.");
        }

        SetRandomSituation(m_situation, m_random);
    }

    std::vector<int> MajorityOnEnvironment::situation() const
    {
        return m_situation;
    }

    double MajorityOnEnvironment::executeAction(int action)
    {
        double reward = (action == getAnswer()) ? 1000.0 : 0.0;

        // Update situation
        SetRandomSituation(m_situation, m_random);

        // Single-step problem
        m_isEndOfProblem = true;

        return reward;
    }

    bool MajorityOnEnvironment::isEndOfProblem() const
    {
        return m_isEndOfProblem;
    }

    int MajorityOnEnvironment::getAnswer() const
    {
        return GetAnswerOfSituation(m_situation);
    }

}
