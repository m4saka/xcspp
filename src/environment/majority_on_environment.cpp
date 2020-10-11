#include "xcspp/environment/majority_on_environment.hpp"
#include <stdexcept>

namespace xcspp
{

    namespace
    {
        std::vector<bool> RandomSituation(std::size_t length)
        {
            std::vector<bool> situation;
            situation.reserve(length);
            for (std::size_t i = 0; i < length; ++i)
            {
                situation.push_back(Random::nextInt(0, 1));
            }
            return situation;
        }
    }

    MajorityOnEnvironment::MajorityOnEnvironment(std::size_t length)
        : m_length(length)
        , m_situation(RandomSituation(length))
        , m_isEndOfProblem(false)
    {
        if ((m_length % 2) == 0)
        {
            throw std::invalid_argument("The length parameter of MajorityOnEnvironment must be an odd number.");
        }
    }

    std::vector<bool> MajorityOnEnvironment::situation() const
    {
        return m_situation;
    }

    double MajorityOnEnvironment::executeAction(bool action)
    {
        double reward = (action == getAnswer()) ? 1000.0 : 0.0;

        // Update situation
        m_situation = RandomSituation(m_length);

        // Single-step problem
        m_isEndOfProblem = true;

        return reward;
    }

    bool MajorityOnEnvironment::isEndOfProblem() const
    {
        return m_isEndOfProblem;
    }

    bool MajorityOnEnvironment::getAnswer() const
    {
        std::size_t sum = 0;
        for (bool b : m_situation)
        {
            sum += static_cast<std::size_t>(b);
        }
        return sum > m_length / 2;
    }

    std::unordered_set<int> availableActions() const
    {
        return { 0, 1 };
    }

}
