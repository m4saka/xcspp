#include "xcspp/classifier.hpp"
#include <utility> // std::move
#include <cstddef> // std::size_t
#include <cmath> // std::pow

namespace xcspp
{

    ConditionActionPair::ConditionActionPair(const Condition & condition, int action)
        : condition(condition)
        , action(action)
    {
    }

    ConditionActionPair::ConditionActionPair(Condition && condition, int action)
        : condition(std::move(condition))
        , action(action)
    {
    }

    std::ostream & operator<< (std::ostream & os, const ConditionActionPair & obj)
    {
        return os << obj.condition << ':' << obj.action;
    }

    Classifier::Classifier(const Condition & condition, int action, double prediction, double epsilon, double fitness, std::uint64_t timeStamp)
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

    Classifier::Classifier(const ConditionActionPair & conditionActionPair, double prediction, double epsilon, double fitness, std::uint64_t timeStamp)
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

    Classifier::Classifier(ConditionActionPair && conditionActionPair, double prediction, double epsilon, double fitness, std::uint64_t timeStamp)
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

    Classifier::Classifier(const std::vector<int> & situation, int action, double prediction, double epsilon, double fitness, std::uint64_t timeStamp)
        : Classifier(Condition(situation), action, prediction, epsilon, fitness, timeStamp)
    {
    }

    Classifier::Classifier(const std::string & condition, int action, double prediction, double epsilon, double fitness, std::uint64_t timeStamp)
        : Classifier(Condition(condition), action, prediction, epsilon, fitness, timeStamp)
    {
    }

    double Classifier::accuracy(double epsilonZero, double alpha, double nu) const
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

    StoredClassifier::StoredClassifier(const Classifier & obj, const XCSParams *pParams)
        : Classifier(obj)
        , m_pParams(pParams)
    {
    }

    StoredClassifier::StoredClassifier(const Condition & condition, int action, std::uint64_t timeStamp, const XCSParams *pParams)
        : Classifier(condition, action, pParams->initialPrediction, pParams->initialEpsilon, pParams->initialFitness, timeStamp)
        , m_pParams(pParams)
    {
    }

    StoredClassifier::StoredClassifier(const ConditionActionPair & conditionActionPair, std::uint64_t timeStamp, const XCSParams *pParams)
        : Classifier(conditionActionPair, pParams->initialPrediction, pParams->initialEpsilon, pParams->initialFitness, timeStamp)
        , m_pParams(pParams)
    {
    }

    StoredClassifier::StoredClassifier(ConditionActionPair && conditionActionPair, std::uint64_t timeStamp, const XCSParams *pParams)
        : Classifier(std::move(conditionActionPair), pParams->initialPrediction, pParams->initialEpsilon, pParams->initialFitness, timeStamp)
        , m_pParams(pParams)
    {
    }

    StoredClassifier::StoredClassifier(const std::vector<int> & situation, int action, std::uint64_t timeStamp, const XCSParams *pParams)
        : Classifier(situation, action, pParams->initialPrediction, pParams->initialEpsilon, pParams->initialFitness, timeStamp)
        , m_pParams(pParams)
    {
    }

    StoredClassifier::StoredClassifier(const std::string & condition, int action, std::uint64_t timeStamp, const XCSParams *pParams)
        : Classifier(condition, action, pParams->initialPrediction, pParams->initialEpsilon, pParams->initialFitness, timeStamp)
        , m_pParams(pParams)
    {
    }

    // COULD SUBSUME
    bool StoredClassifier::isSubsumer() const
    {
        return experience > m_pParams->thetaSub && epsilon < m_pParams->epsilonZero;
    }

    // DOES SUBSUME
    bool StoredClassifier::subsumes(const Classifier & cl) const
    {
        return action == cl.action && isSubsumer() && condition.isMoreGeneral(cl.condition);
    }

    double StoredClassifier::accuracy() const
    {
        return Classifier::accuracy(m_pParams->epsilonZero, m_pParams->alpha, m_pParams->nu);
    }

}
