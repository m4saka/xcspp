#include "xcspp/environment/majority_on_environment.hpp"
#include <stdexcept>

namespace xcspp
{

    namespace
    {
        std::vector<int> RandomSituation(Random & random, std::size_t length)
        {
            std::vector<int> situation;
            situation.reserve(length);
            for (std::size_t i = 0; i < length; ++i)
            {
                situation.push_back(random.nextInt(0, 1));
            }
            return situation;
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
        , m_situation(RandomSituation(m_random, length))
        , m_isEndOfProblem(false)
    {
        if ((m_length % 2) == 0)
        {
            throw std::invalid_argument("The length parameter of MajorityOnEnvironment must be an odd number.");
        }
    }

    std::vector<int> MajorityOnEnvironment::situation() const
    {
        return m_situation;
    }

    double MajorityOnEnvironment::executeAction(int action)
    {
        double reward = (action == getAnswer()) ? 1000.0 : 0.0;

        // Update situation
        m_situation = RandomSituation(m_random, m_length);

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
