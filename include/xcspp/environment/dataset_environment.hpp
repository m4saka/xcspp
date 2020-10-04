#pragma once
#include <vector>
#include <string>
#include <unordered_set>
#include <cstddef>
#include <cassert>

#include "ienvironment.hpp"
#include "xcspp/random.hpp"
#include "xcspp/util/dataset.hpp"

namespace xcspp
{

    class DatasetEnvironment : public IEnvironment
    {
    protected:
        const Dataset m_dataset;
        const std::unordered_set<int> m_availableActions;
        std::vector<int> m_situation;
        int m_answer;
        std::size_t m_nextIdx;
        const bool m_chooseRandom;
        bool m_isEndOfProblem;
        Random m_random;

        std::size_t loadNext()
        {
            if (m_chooseRandom)
            {
                const auto idx = m_random.nextInt<std::size_t>(0UL, m_dataset.situations.size() - 1UL);
                m_situation = m_dataset.situations[idx];
                m_answer = m_dataset.actions[idx];
                return idx;
            }
            else
            {
                m_situation = m_dataset.situations[m_nextIdx];
                m_answer = m_dataset.actions[m_nextIdx];
                const auto idx = m_nextIdx;
                if (++m_nextIdx >= m_dataset.situations.size())
                {
                    m_nextIdx = 0;
                }
                return idx;
            }
        }

    public:
        DatasetEnvironment(const Dataset & dataset, const std::unordered_set<int> & availableActions, bool chooseRandom = true)
            : m_dataset(dataset)
            , m_availableActions(availableActions)
            , m_nextIdx(0)
            , m_chooseRandom(chooseRandom)
            , m_isEndOfProblem(false)
        {
            assert(!m_dataset.situations.empty());
            assert(!m_dataset.actions.empty());
            assert(m_dataset.situations.size() == m_dataset.actions.size());

            loadNext();
        }

        virtual ~DatasetEnvironment() = default;

        virtual std::vector<int> situation() const override
        {
            return m_situation;
        }

        virtual double executeAction(int action) override
        {
            const double reward = (action == m_answer) ? 1000.0 : 0.0;

            // Single-step problem
            m_isEndOfProblem = true;

            loadNext();

            return reward;
        }

        virtual bool isEndOfProblem() const override
        {
            return m_isEndOfProblem;
        }

        virtual std::unordered_set<int> availableActions() const override
        {
            return m_availableActions;
        }

        // Returns the answer
        int getAnswer() const
        {
            return m_answer;
        }
    };

}
