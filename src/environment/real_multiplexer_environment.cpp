#include "xcspp/environment/real_multiplexer_environment.hpp"
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

        int GetAnswerOfSituation(const std::vector<double> & situation, double binaryThreshold)
        {
            std::size_t address = 0;
            const auto addressBitLength = AddressBitLength(situation.size());
            for (std::size_t i = 0; i < addressBitLength; ++i)
            {
                if (situation[i] >= binaryThreshold)
                {
                    address += std::size_t{1} << (addressBitLength - i - std::size_t{1});
                }
            }

            return situation[addressBitLength + address] >= binaryThreshold ? 1 : 0;
        }

        void SetRandomSituation(std::vector<double> & situation, Random & random, double binaryThreshold, double minorityAcceptanceProbability = 1.0)
        {
            while (true)
            {
                for (auto & s : situation)
                {
                    s = random.nextDouble();
                }

                if (GetAnswerOfSituation(situation, binaryThreshold) == 1)
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

    RealMultiplexerEnvironment::RealMultiplexerEnvironment(std::size_t length, double binaryThreshold, unsigned int imbalanceLevel)
        : m_binaryThreshold(binaryThreshold)
        , m_minorityAcceptanceProbability(1.0 / std::pow(2, imbalanceLevel))
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

    std::vector<double> RealMultiplexerEnvironment::situation() const
    {
        return m_situation;
    }

    double RealMultiplexerEnvironment::executeAction(int action)
    {
        double reward = (action == getAnswer()) ? 1000.0 : 0.0;

        // Update situation
        SetRandomSituation(m_situation, m_random, m_minorityAcceptanceProbability);

        // In single-step problem, isEndOfProblem() always returns true after the first execution
        m_isEndOfProblem = true;

        return reward;
    }

    bool RealMultiplexerEnvironment::isEndOfProblem() const
    {
        return m_isEndOfProblem;
    }

    int RealMultiplexerEnvironment::getAnswer() const
    {
        return GetAnswerOfSituation(m_situation, m_binaryThreshold);
    }

}
