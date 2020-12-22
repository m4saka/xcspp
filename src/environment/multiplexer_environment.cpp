#include "xcspp/environment/multiplexer_environment.hpp"
#include <stdexcept>
#include <cmath> // std::pow

namespace xcspp
{

    namespace
    {
        // Get address bit length from total length
        constexpr std::size_t AddressBitLength(std::size_t l, std::size_t c = 0)
        {
            return (l == 0) ? c - 1 : AddressBitLength(l >> 1, c + 1);
        }

        int GetAnswerOfSituation(const std::vector<int> & situation)
        {
            std::size_t address = 0;
            const auto addressBitLength = AddressBitLength(situation.size());
            for (std::size_t i = 0; i < addressBitLength; ++i)
            {
                if (situation[i] == 1)
                {
                    address += std::size_t{1} << (addressBitLength - i - std::size_t{1});
                }
            }

            return situation[addressBitLength + address];
        }

        void SetRandomSituation(std::vector<int> & situation, Random & random, double minorityAcceptanceProbability = 1.0)
        {
            while (true)
            {
                for (auto & s: situation)
                {
                    s = random.nextInt(0, 1);
                }

                if (GetAnswerOfSituation(situation) == 1)
                {
                    break;
                }
                else if (random.nextDouble() < minorityAcceptanceProbability)
                {
                    break;
                }
            }
        }
    }

    MultiplexerEnvironment::MultiplexerEnvironment(std::size_t length, unsigned int imbalanceLevel)
        : m_minorityAcceptanceProbability(1.0 / std::pow(2, imbalanceLevel))
        , m_situation(length)
        , m_isEndOfProblem(false)
    {
        // Total length must be n + 2^n (n > 0)
        const auto addressBitLength = AddressBitLength(length);
        if (length != (addressBitLength + (std::size_t{1} << addressBitLength)))
        {
            throw std::invalid_argument("The input length of multiplexer problem must be n + 2^n (n > 0)");
        }

        SetRandomSituation(m_situation, m_random, m_minorityAcceptanceProbability);
    }

    std::vector<int> MultiplexerEnvironment::situation() const
    {
        return m_situation;
    }

    double MultiplexerEnvironment::executeAction(int action)
    {
        const double reward = (action == getAnswer()) ? 1000.0 : 0.0;

        // Update situation
        SetRandomSituation(m_situation, m_random, m_minorityAcceptanceProbability);

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
        return GetAnswerOfSituation(m_situation);
    }

}
