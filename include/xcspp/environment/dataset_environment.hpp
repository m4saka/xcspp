#pragma once
#include <vector>
#include <unordered_set>
#include <cstddef>

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

        std::size_t loadNext();

    public:
        DatasetEnvironment(const Dataset & dataset, const std::unordered_set<int> & availableActions, bool chooseRandom = true);

        virtual ~DatasetEnvironment() = default;

        virtual std::vector<int> situation() const override;

        virtual double executeAction(int action) override;

        virtual bool isEndOfProblem() const override;

        virtual std::unordered_set<int> availableActions() const override;

        // Returns the answer
        int getAnswer() const;
    };

}
