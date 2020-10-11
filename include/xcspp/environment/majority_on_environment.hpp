#pragma once
#include <vector>
#include <cstddef> // std::size_t

#include "ienvironment.hpp"
#include "xcspp/random.hpp"

namespace xcspp
{

    class MajorityOnEnvironment : public IEnvironment
    {
    private:
        const std::size_t m_length;
        std::vector<int> m_situation;
        bool m_isEndOfProblem;
        Random m_random;

    public:
        // Constructor
        explicit MajorityOnEnvironment(std::size_t length);

        // Destructor
        virtual ~MajorityOnEnvironment() = default;

        // Returns current situation
        virtual std::vector<int> situation() const override;

        // Executes action (and update situation), and returns reward
        virtual double executeAction(int action) override;

        // Returns true if the problem was solved by the previous action
        // (Since it is a single-step problem, this function always returns true after the first action execution)
        virtual bool isEndOfProblem() const override;

        // Returns available action choices
        virtual std::unordered_set<int> availableActions() const override
        {
            return { 0, 1 };
        }

        // Returns answer to situation
        int getAnswer() const;
    };

}
