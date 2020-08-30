#pragma once

#include <vector>
#include <cmath>
#include <cstddef>
#include <cassert>

#include "ienvironment.hpp"
#include "xcscpp/random.hpp"

namespace xcscpp
{

    class MultiplexerEnvironment : public IEnvironment
    {
    private:
        const std::size_t m_totalLength;
        const std::size_t m_addressBitLength;
        const double m_minorityAcceptanceProbability;
        std::vector<int> m_situation;
        bool m_isEndOfProblem;

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

    public:
        // Constructor
        // (Set imbalanceLevel to higher than 0 for imbalanced multiplexer problems)
        explicit MultiplexerEnvironment(std::size_t length, unsigned int imbalanceLevel = 0)
            : m_totalLength(length)
            , m_addressBitLength(addressBitLength(length, 0))
            , m_minorityAcceptanceProbability(1.0 / std::pow(2, imbalanceLevel))
            , m_situation(randomSituation(length, m_minorityAcceptanceProbability))
            , m_isEndOfProblem(false)
        {
            // Total length must be n + 2^n (n > 0)
            assert(m_totalLength == (m_addressBitLength + ((std::size_t)1 << m_addressBitLength)));
        }

        virtual ~MultiplexerEnvironment() = default;

        virtual std::vector<int> situation() const override
        {
            return m_situation;
        }

        virtual double executeAction(int action) override
        {
            double reward = (action == getAnswer()) ? 1000.0 : 0.0;

            // Update situation
            m_situation = randomSituation(m_totalLength, m_minorityAcceptanceProbability);

            // Single-step problem
            m_isEndOfProblem = true;

            return reward;
        }

        virtual bool isEndOfProblem() const override
        {
            return m_isEndOfProblem;
        }

        virtual std::unordered_set<int> availableActions() const override
        {
            return { 0, 1 };
        }

        // Returns answer to situation
        int getAnswer() const
        {
            return getAnswerOfSituation(m_situation);
        }
    };

}
