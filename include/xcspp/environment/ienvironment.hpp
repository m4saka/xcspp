#pragma once
#include <vector>
#include <unordered_set>

namespace xcspp
{

    // Environment interface
    //   If you would like to use your own enviroment/problem with ExperimentHelper, 
    //   your class should implement this interface.
    template <typename T>
    class IBasicEnvironment
    {
    public:
        IBasicEnvironment() = default;

        virtual ~IBasicEnvironment() = default;

        // Returns current situation
        virtual std::vector<T> situation() const = 0;

        // Executes action (and updates situation), and returns reward
        virtual double executeAction(int action) = 0;

        // Returns true if the problem was solved by the previous action
        // (always true for a single-step problem after the first action execution)
        virtual bool isEndOfProblem() const = 0;

        // Returns available action choices (e.g. { 0, 1 })
        virtual std::unordered_set<int> availableActions() const = 0;
    };

    // Environment interface for XCS
    using IEnvironment = IBasicEnvironment<int>;

    // Environment interface for XCSR
    using IRealEnvironment = IBasicEnvironment<double>;

}
