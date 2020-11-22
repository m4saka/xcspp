#pragma once
#include <string>
#include <vector>
#include <cstdint> // std::uint64_t

#include "condition.hpp"
#include "xcs_params.hpp"

namespace xcspp::xcs
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

        ConditionActionPair(const Condition & condition, int action);

        ConditionActionPair(Condition && condition, int action);

        // Destructor
        virtual ~ConditionActionPair() = default;

        friend std::ostream & operator<< (std::ostream & os, const ConditionActionPair & obj);
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

        Classifier(const Condition & condition, int action, double prediction, double epsilon, double fitness, std::uint64_t timeStamp);

        Classifier(const ConditionActionPair & conditionActionPair, double prediction, double epsilon, double fitness, std::uint64_t timeStamp);

        Classifier(ConditionActionPair && conditionActionPair, double prediction, double epsilon, double fitness, std::uint64_t timeStamp);

        Classifier(const std::vector<int> & situation, int action, double prediction, double epsilon, double fitness, std::uint64_t timeStamp);

        Classifier(const std::string & condition, int action, double prediction, double epsilon, double fitness, std::uint64_t timeStamp);

        // Destructor
        virtual ~Classifier() = default;

        double accuracy(double epsilonZero, double alpha, double nu) const;
    };

    // Classifier in [P] (have a reference to XCSParams)
    struct StoredClassifier : Classifier
    {
    private:
        // XCSParams
        const XCSParams * const m_pParams;

    public:
        // Constructor
        StoredClassifier(const StoredClassifier & obj) = default;

        StoredClassifier(const Classifier & obj, const XCSParams *pParams);

        StoredClassifier(const Condition & condition, int action, std::uint64_t timeStamp, const XCSParams *pParams);

        StoredClassifier(const ConditionActionPair & conditionActionPair, std::uint64_t timeStamp, const XCSParams *pParams);

        StoredClassifier(ConditionActionPair && conditionActionPair, std::uint64_t timeStamp, const XCSParams *pParams);

        StoredClassifier(const std::vector<int> & situation, int action, std::uint64_t timeStamp, const XCSParams *pParams);

        StoredClassifier(const std::string & condition, int action, std::uint64_t timeStamp, const XCSParams *pParams);

        // Destructor
        virtual ~StoredClassifier() = default;

        // COULD SUBSUME
        bool isSubsumer() const;

        // DOES SUBSUME
        bool subsumes(const Classifier & cl) const;

        double accuracy() const;
    };

}
