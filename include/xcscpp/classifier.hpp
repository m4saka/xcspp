#pragma once
#include <string>
#include <unordered_set>
#include <memory>
#include <cstdint> // std::uint64_t
#include <cstddef> // std::size_t
#include <cmath>

#include "condition.hpp"
#include "constants.hpp"

namespace xcscpp
{

    struct ConditionActionPair
    {
    public:
        // C
        //   The condition specifies the input states (sensory situations)
        //   in which the classifier can be applied (matches).
        Condition condition;

        // A
        //   The action specifies the action (possibly a classification)
        //   that the classifier proposes.
        int action;

        // Constructor
        ConditionActionPair(const ConditionActionPair &) = default;

        ConditionActionPair(const Condition & condition, int action) : condition(condition), action(action) {}

        ConditionActionPair(Condition && condition, int action) : condition(std::move(condition)), action(action) {}

        // Destructor
        virtual ~ConditionActionPair() = default;

        // IS MORE GENERAL
        bool isMoreGeneral(const ConditionActionPair & cl) const
        {
            if (condition.size() != cl.condition.size())
            {
                std::invalid_argument("ConditionActionPair::isMoreGeneral() could not process the classifier with a different condition length.");
            }

            bool ret = false;

            for (std::size_t i = 0; i < condition.size(); ++i)
            {
                if (condition.at(i) != cl.condition.at(i))
                {
                    if (!condition.at(i).isDontCare())
                    {
                        return false;
                    }
                    else 
                    {
                        ret = true;
                    }
                }
            }

            return ret;
        }

        friend std::ostream & operator<< (std::ostream & os, const ConditionActionPair & obj)
        {
            return os << obj.condition << ':' << obj.action;
        }
    };

    struct Classifier : ConditionActionPair
    {
    public:
        // p
        //   The prediction p estimates (keeps an average of) the payoff expected if the
        //   classifier matches and its action is taken by the system.
        double prediction;

        // epsilon
        //   The prediction error epsilon estimates the errors made in the predictions.
        double epsilon;

        // F
        //   The fitness F denotes the classifier's fitness.
        double fitness;

        // exp
        //   The experience exp counts the number of times since its creation that the
        //   classifier has belonged to an action set.
        std::uint64_t experience;

        // ts
        //   The time stamp ts denotes the time-step of the last occurrence of a GA in
        //   an action set to which this classifier belonged.
        std::uint64_t timeStamp;

        // as
        //   The action set size as estimates the average size of the action sets this
        //   classifier has belonged to.
        double actionSetSize;

        // n
        //   The numerosity n reflects the number of micro-classifiers (ordinary
        //   classifiers) this classifier - which is technically called a macro-
        //   classifier - represents.
        std::uint64_t numerosity;

        // Constructor
        Classifier(const Classifier &) = default;

        Classifier(const Condition & condition, int action, double prediction, double epsilon, double fitness, std::uint64_t timeStamp)
            : ConditionActionPair(condition, action)
            , prediction(prediction)
            , epsilon(epsilon)
            , fitness(fitness)
            , experience(0)
            , timeStamp(timeStamp)
            , actionSetSize(1)
            , numerosity(1)
        {
        }

        Classifier(const ConditionActionPair & conditionActionPair, double prediction, double epsilon, double fitness, std::uint64_t timeStamp)
            : ConditionActionPair(conditionActionPair)
            , prediction(prediction)
            , epsilon(epsilon)
            , fitness(fitness)
            , experience(0)
            , timeStamp(timeStamp)
            , actionSetSize(1)
            , numerosity(1)
        {
        }

        Classifier(ConditionActionPair && conditionActionPair, double prediction, double epsilon, double fitness, std::uint64_t timeStamp)
            : ConditionActionPair(std::move(conditionActionPair))
            , prediction(prediction)
            , epsilon(epsilon)
            , fitness(fitness)
            , experience(0)
            , timeStamp(timeStamp)
            , actionSetSize(1)
            , numerosity(1)
        {
        }

        Classifier(const std::vector<int> & situation, int action, double prediction, double epsilon, double fitness, std::uint64_t timeStamp)
            : Classifier(Condition(situation), action, prediction, epsilon, fitness, timeStamp)
        {
        }

        Classifier(const std::string & condition, int action, double prediction, double epsilon, double fitness, std::uint64_t timeStamp)
            : Classifier(Condition(condition), action, prediction, epsilon, fitness, timeStamp)
        {
        }

        // Destructor
        virtual ~Classifier() = default;

        double accuracy(double epsilonZero, double alpha, double nu) const
        {
            if (epsilon < epsilonZero)
            {
                return 1.0;
            }
            else
            {
                return alpha * std::pow(epsilon / epsilonZero, -nu);
            }
        }
    };

    // Classifier in [P] (have a reference to Constants)
    struct StoredClassifier : Classifier
    {
    protected:
        // Constants
        Constants *m_pConstants;

    public:
        // Constructor
        StoredClassifier(const StoredClassifier & obj) = default;

        StoredClassifier(const Classifier & obj, Constants *pConstants)
            : Classifier(obj)
            , m_pConstants(pConstants)
        {
        }

        StoredClassifier(const Condition & condition, int action, std::uint64_t timeStamp, Constants *pConstants)
            : Classifier(condition, action, pConstants->initialPrediction, pConstants->initialEpsilon, pConstants->initialFitness, timeStamp)
            , m_pConstants(pConstants)
        {
        }

        StoredClassifier(const ConditionActionPair & conditionActionPair, std::uint64_t timeStamp, Constants *pConstants)
            : Classifier(conditionActionPair, pConstants->initialPrediction, pConstants->initialEpsilon, pConstants->initialFitness, timeStamp)
            , m_pConstants(pConstants)
        {
        }

        StoredClassifier(ConditionActionPair && conditionActionPair, std::uint64_t timeStamp, Constants *pConstants)
            : Classifier(std::move(conditionActionPair), pConstants->initialPrediction, pConstants->initialEpsilon, pConstants->initialFitness, timeStamp)
            , m_pConstants(pConstants)
        {
        }

        StoredClassifier(const std::vector<int> & situation, int action, std::uint64_t timeStamp, Constants *pConstants)
            : Classifier(situation, action, pConstants->initialPrediction, pConstants->initialEpsilon, pConstants->initialFitness, timeStamp)
            , m_pConstants(pConstants)
        {
        }

        StoredClassifier(const std::string & condition, int action, std::uint64_t timeStamp, Constants *pConstants)
            : Classifier(condition, action, pConstants->initialPrediction, pConstants->initialEpsilon, pConstants->initialFitness, timeStamp)
            , m_pConstants(pConstants)
        {
        }

        // Destructor
        virtual ~StoredClassifier() = default;

        // COULD SUBSUME
        bool isSubsumer() const noexcept
        {
            return experience > m_pConstants->thetaSub && epsilon < m_pConstants->epsilonZero;
        }

        // DOES SUBSUME
        bool subsumes(const Classifier & cl) const
        {
            return action == cl.action && isSubsumer() && isMoreGeneral(cl);
        }

        double accuracy() const
        {
            return Classifier::accuracy(m_pConstants->epsilonZero, m_pConstants->alpha, m_pConstants->nu);
        }
    };

}
