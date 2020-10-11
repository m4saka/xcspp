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
        std::vector<bool> m_situation;
        bool m_isEndOfProblem;
        Random m_random;

    public:
        // Constructor
        explicit MajorityOnEnvironment(std::size_t length);

        // Destructor
        virtual ~MajorityOnEnvironment() = default;

        // Returns current situation
        virtual std::vector<bool> situation() const override;

        // Executes action (and update situation), and returns reward
        virtual double executeAction(bool action) override;

        // Returns true if the problem was solved by the previous action
        // (Since it is a single-step problem, this function always returns true after the first action execution)
        virtual bool isEndOfProblem() const override;

        // Returns answer to situation
        bool getAnswer() const;

        // Returns available action choices (e.g. { 0, 1 })
        virtual std::unordered_set<int> availableActions() const override;
    };

}
