#pragma once
#include <iosfwd> // std::ostream
#include <string>
#include <cstddef> // std::size_t

namespace xcspp
{

    // Learning classifier system interface
    template <typename T>
    class IBasicClassifierSystem
    {
    public:
        // Constructor
        IBasicClassifierSystem() = default;

        // Destructor
        virtual ~IBasicClassifierSystem() = default;

        // Run with exploration
        virtual int explore(const std::vector<T> & situation) = 0;

        // Feedback reward to system
        virtual void reward(double value, bool isEndOfProblem = true) = 0;

        // Run without exploration
        // (Set update to true when testing multi-step problems. If update is true, make sure to call reward() after this.)
        virtual int exploit(const std::vector<T> & situation, bool update = false) = 0;

        // Get prediction value of the previous action decision
        // (Call this function after explore() or exploit())
        virtual double prediction() const = 0;

        // Get prediction value of the action
        // (Call this function after explore() or exploit())
        virtual double predictionFor(int action) const = 0;

        // Get if covering is performed in the previous action decision
        // (Call this function after explore() or exploit())
        virtual bool isCoveringPerformed() const = 0;

        virtual void outputPopulationCSV(std::ostream & os) const = 0;

        virtual bool loadPopulationCSVFile(const std::string & filename, bool initClassifierVariables = false, bool syncTimeStamp = true) = 0;

        virtual bool savePopulationCSVFile(const std::string & filename) const = 0;

        virtual std::size_t populationSize() const = 0;

        virtual std::size_t numerositySum() const = 0;

        virtual void switchToCondensationMode() = 0;
    };

    using IClassifierSystem = IBasicClassifierSystem<int>;
    using IRealClassifierSystem = IBasicClassifierSystem<double>;

}
