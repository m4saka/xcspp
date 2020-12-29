#pragma once
#include <vector>
#include <unordered_set>
#include <cstddef>

#include "ienvironment.hpp"
#include "xcspp/util/random.hpp"
#include "xcspp/util/dataset.hpp"

namespace xcspp
{

    template <typename T>
    class BasicDatasetEnvironment : public IBasicEnvironment<T>
    {
    protected:
        const BasicDataset<T> m_dataset;
        const std::unordered_set<int> m_availableActions;
        std::vector<T> m_situation;
        int m_answer;
        std::size_t m_nextIdx;
        const bool m_chooseRandom;
        bool m_isEndOfProblem;
        Random m_random;

        std::size_t loadNext();

    public:
        BasicDatasetEnvironment(const BasicDataset<T> & dataset, bool chooseRandom = true);

        virtual ~BasicDatasetEnvironment() = default;

        virtual std::vector<T> situation() const override;

        virtual double executeAction(int action) override;

        virtual bool isEndOfProblem() const override;

        virtual std::unordered_set<int> availableActions() const override;

        // Returns the answer
        int getAnswer() const;
    };

    using DatasetEnvironment = BasicDatasetEnvironment<int>;
    using RealDatasetEnvironment = BasicDatasetEnvironment<double>;

    namespace detail
    {
        template <typename T>
        std::unordered_set<int> GetAvailableActionsInDataset(const BasicDataset<T> & dataset)
        {
            std::unordered_set<int> availableActions;
            for (const auto & action : dataset.actions)
            {
                availableActions.insert(action); // already inserted items are ignored here
            }
            return availableActions;
        }
    }

    template <typename T>
    std::size_t BasicDatasetEnvironment<T>::loadNext()
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

    template <typename T>
    BasicDatasetEnvironment<T>::BasicDatasetEnvironment(const BasicDataset<T> & dataset, bool chooseRandom)
        : m_dataset(dataset)
        , m_availableActions(detail::GetAvailableActionsInDataset(dataset))
        , m_nextIdx(0)
        , m_chooseRandom(chooseRandom)
        , m_isEndOfProblem(false)
    {
        if (m_dataset.situations.size() != m_dataset.actions.size())
        {
            throw std::domain_error("The dataset size is invalid in DatasetEnvironment constructor (situations/actions size does not match).");
        }

        if (m_dataset.situations.empty() || m_dataset.actions.empty())
        {
            throw std::runtime_error("DatasetEnvironment constructor received an empty dataset.");
        }

        loadNext();
    }

    template <typename T>
    std::vector<T> BasicDatasetEnvironment<T>::situation() const
    {
        return m_situation;
    }

    template <typename T>
    double BasicDatasetEnvironment<T>::executeAction(int action)
    {
        const double reward = (action == m_answer) ? 1000.0 : 0.0;

        // Single-step problem
        m_isEndOfProblem = true;

        loadNext();

        return reward;
    }

    template <typename T>
    bool BasicDatasetEnvironment<T>::isEndOfProblem() const
    {
        return m_isEndOfProblem;
    }

    template <typename T>
    std::unordered_set<int> BasicDatasetEnvironment<T>::availableActions() const
    {
        return m_availableActions;
    }

    template <typename T>
    int BasicDatasetEnvironment<T>::getAnswer() const
    {
        return m_answer;
    }
}
