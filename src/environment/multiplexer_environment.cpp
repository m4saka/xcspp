#include "xcspp/environment/multiplexer_environment.hpp"
#include <stdexcept>
#include <cmath> // std::pow

#include "xcspp/random.hpp"

namespace xcspp
{

    // Get address bit length from total length
    static constexpr std::size_t addressBitLength(std::size_t l, std::size_t c = 0)
    {
        return (l == 0) ? c - 1 : addressBitLength(l >> 1, c + 1);
    }

    static int getAnswerOfSituation(const std::vector<int> & situation)
    {
        std::size_t address = 0;
        auto l = addressBitLength(situation.size());
        for (std::size_t i = 0; i < l; ++i)
        {
            if (situation.at(i) == 1)
            {
                address += (std::size_t)1 << (l - i - 1);
            }
        }

        return situation.at(l + address);
    }

    static std::vector<int> randomSituation(std::size_t totalLength, double minorityAcceptanceProbability = 1.0)
    {
        std::vector<int> situation;
        situation.reserve(totalLength);

        while (true)
        {
            for (std::size_t i = 0; i < totalLength; ++i)
            {
                situation.push_back(Random::nextInt(0, 1));
            }

            if (getAnswerOfSituation(situation) == 1)
            {
                break;
            }
            else if (Random::nextDouble() < minorityAcceptanceProbability)
            {
                break;
            }
            else
            {
                situation.clear();
            }
        }
        return situation;
    }

    MultiplexerEnvironment::MultiplexerEnvironment(std::size_t length, unsigned int imbalanceLevel)
        : m_totalLength(length)
        , m_addressBitLength(addressBitLength(length, 0))
        , m_minorityAcceptanceProbability(1.0 / std::pow(2, imbalanceLevel))
        , m_situation(randomSituation(length, m_minorityAcceptanceProbability))
        , m_isEndOfProblem(false)
    {
        // Total length must be n + 2^n (n > 0)
        if (m_totalLength != (m_addressBitLength + ((std::size_t)1 << m_addressBitLength)))
        {
            throw std::invalid_argument("The input length of multiplexer problem must be n + 2^n (n > 0)");
        }
    }

    std::vector<int> MultiplexerEnvironment::situation() const
    {
        return m_situation;
    }

    double MultiplexerEnvironment::executeAction(int action)
    {
        double reward = (action == getAnswer()) ? 1000.0 : 0.0;

        // Update situation
        m_situation = randomSituation(m_totalLength, m_minorityAcceptanceProbability);

        // In single-step problem, isEndOfProblem() always returns true after the first execution
        m_isEndOfProblem = true;

        return reward;
    }

    bool MultiplexerEnvironment::isEndOfProblem() const
    {
        return m_isEndOfProblem;
    }

    int MultiplexerEnvironment::getAnswer() const
    {
        return getAnswerOfSituation(m_situation);
    }

}
