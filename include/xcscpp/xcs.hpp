#pragma once
#include <iostream>
#include <sstream>
#include <algorithm>
#include <memory>
#include <vector>
#include <cstdint> // std::uint64_t
#include <cstddef> // std::size_t
#include <cmath>

#include "constants.hpp"
#include "population.hpp"
#include "match_set.hpp"
#include "action_set.hpp"
#include "prediction_array.hpp"
#include "helper/csv.hpp"

namespace xcscpp
{

    class XCS
    {
    protected:
        // Hyperparameters
        Constants m_constants;

        // [P]
        //   The population [P] consists of all classifier that exist in XCS at any time.
        Population m_population;

        // [A]
        //   The action set [A] is formed out of the current [M].
        //   It includes all classifiers of [M] that propose the executed action.
        ActionSet m_actionSet;

        // [A]_-1
        //   The previous action set [A]_-1 is the action set that was active in the last
        //   execution cycle.
        ActionSet m_prevActionSet;

        // Available action choices
        std::unordered_set<int> m_availableActions;

        std::uint64_t m_timeStamp;

        bool m_expectsReward;
        double m_prevReward;
        bool m_isPrevModeExplore;

        std::vector<int> m_prevSituation;

        // Prediction value of the previous action decision (just for logging)
        double m_prediction;
        std::unordered_map<int, double> m_predictions;

        // Covering occurrence of the previous action decision (just for logging)
        bool m_isCoveringPerformed;

    public:
        // Constructor
        XCS(const std::unordered_set<int> & availableActions, const Constants & constants)
            : m_constants(constants)
            , m_population(&m_constants, availableActions)
            , m_actionSet(&m_constants, availableActions)
            , m_prevActionSet(&m_constants, availableActions)
            , m_availableActions(availableActions)
            , m_timeStamp(0)
            , m_expectsReward(false)
            , m_prevReward(0.0)
            , m_isPrevModeExplore(false)
            , m_prediction(0.0)
            , m_isCoveringPerformed(false)
        {
        }

        // Destructor
        ~XCS() = default;

        // Run with exploration
        int explore(const std::vector<int> & situation)
        {
            if (m_expectsReward)
            {
                throw std::domain_error("XCS::explore() is called although XCS expects reward() to be called.");
            }

            // [M]
            //   The match set [M] is formed out of the current [P].
            //   It includes all classifiers that match the current situation.
            const MatchSet matchSet(m_population, situation, m_timeStamp, &m_constants, m_availableActions);
            m_isCoveringPerformed = matchSet.isCoveringPerformed();

            const PredictionArray predictionArray(matchSet);

            const int action = predictionArray.selectAction(m_constants.exploreProbability);
            m_prediction = predictionArray.predictionFor(action);
            for (const auto & action : m_availableActions)
            {
                m_predictions[action] = predictionArray.predictionFor(action);
            }

            m_actionSet.regenerate(matchSet, action);

            m_expectsReward = true;
            m_isPrevModeExplore = true;

            if (!m_prevActionSet.empty())
            {
                double p = m_prevReward + m_constants.gamma * predictionArray.max();
                m_prevActionSet.update(p, m_population);
                m_prevActionSet.runGA(m_prevSituation, m_population, m_timeStamp);
            }

            m_prevSituation = situation;

            return action;
        }

        void reward(double value, bool isEndOfProblem = true)
        {
            if (!m_expectsReward)
            {
                throw std::domain_error("XCS::reward() is called although XCS::explore() is not called after the previous reward() call.");
            }

            if (isEndOfProblem)
            {
                m_actionSet.update(value, m_population);
                if (m_isPrevModeExplore) // Do not perform GA operations in exploitation
                {
                    m_actionSet.runGA(m_prevSituation, m_population, m_timeStamp);
                }
                m_prevActionSet.clear();
            }
            else
            {
                m_actionSet.copyTo(m_prevActionSet);
                m_prevReward = value;
            }

            if (m_isPrevModeExplore) // Do not increment actual time in exploitation
            {
                ++m_timeStamp;
            }

            m_expectsReward = false;
        }

        // Run without exploration
        // (Set update to true when testing multi-step problems. If update is true, make sure to call reward() after this.)
        int exploit(const std::vector<int> & situation, bool update = false)
        {
            if (update)
            {
                if (m_expectsReward)
                {
                    throw std::domain_error("XCS::explore() is called although XCS expects reward() to be called.");
                }

                // [M]
                //   The match set [M] is formed out of the current [P].
                //   It includes all classifiers that match the current situation.
                const MatchSet matchSet(m_population, situation, m_timeStamp, &m_constants, m_availableActions);
                m_isCoveringPerformed = matchSet.isCoveringPerformed();

                const PredictionArray predictionArray(matchSet);

                const int action = predictionArray.selectAction();

                m_actionSet.regenerate(matchSet, action);

                m_expectsReward = true;
                m_isPrevModeExplore = false;

                if (!m_prevActionSet.empty())
                {
                    double p = m_prevReward + m_constants.gamma * predictionArray.max();
                    m_prevActionSet.update(p, m_population);

                    // Do not perform GA operations in exploitation
                }

                m_prevSituation = situation;

                return action;
            }
            else
            {
                // Create new match set as sandbox
                MatchSet matchSet(&m_constants, m_availableActions);
                for (const auto & cl : m_population)
                {
                    if (cl->condition.matches(situation))
                    {
                        matchSet.insert(cl);
                    }
                }

                if (!matchSet.empty())
                {
                    m_isCoveringPerformed = false;

                    PredictionArray predictionArray(matchSet);
                    const int action = predictionArray.selectAction(m_constants.exploreProbability);
                    m_prediction = predictionArray.predictionFor(action);
                    for (const auto & action : m_availableActions)
                    {
                        m_predictions[action] = predictionArray.predictionFor(action);
                    }
                    return action;
                }
                else
                {
                    m_isCoveringPerformed = true;
                    m_prediction = m_constants.initialPrediction;
                    for (const auto & action : m_availableActions)
                    {
                        m_predictions[action] = m_constants.initialPrediction;
                    }
                    return Random::chooseFrom(m_availableActions);
                }
            }
        }

        // Get prediction value of the previous action decision
        // (Call this function after explore() or exploit())
        double prediction() const
        {
            return m_prediction;
        }

        // Get prediction value of the action
        // (Call this function after explore() or exploit())
        double predictionFor(int action) const
        {
            return m_predictions.at(action);
        }

        // Get if covering is performed in the previous action decision
        // (Call this function after explore() or exploit())
        bool isCoveringPerformed() const
        {
            return m_isCoveringPerformed;
        }

        std::vector<Classifier> getMatchingClassifiers(const std::vector<int> & situation) const
        {
            std::vector<Classifier> classifiers;
            for (const auto & cl : m_population)
            {
                if (cl->condition.matches(situation))
                {
                    classifiers.emplace_back(*cl);
                }
            }
            return classifiers;
        }

        void loadPopulationCSV(const std::string & filename, bool useAsInitialPopulation = true)
        {
            auto population = CSV::readPopulation(filename);
            if (useAsInitialPopulation)
            {
                for (auto & cl : population)
                {
                    cl.prediction = m_constants.initialPrediction;
                    cl.epsilon = m_constants.initialEpsilon;
                    cl.fitness = m_constants.initialFitness;
                    cl.experience = 0;
                    cl.timeStamp = 0;
                    cl.actionSetSize = 1;
                    //cl.numerosity = 1; // commented out to keep macroclassifier as is
                }
            }
            setPopulation(population, !useAsInitialPopulation);
        }

        void setPopulation(const std::vector<Classifier> & classifiers, bool initTimeStamp = true)
        {
            // Replace population
            m_population.clear();
            for (const auto & cl : classifiers)
            {
                m_population.emplace(std::make_shared<StoredClassifier>(cl, &m_constants));
            }

            // Clear action set and reset status
            m_actionSet.clear();
            m_prevActionSet.clear();
            m_expectsReward = false;
            m_isPrevModeExplore = false;

            // Set system timestamp to the same as latest classifier
            if (initTimeStamp)
            {
                m_timeStamp = 0;
                for (const auto & cl : classifiers)
                {
                    if (m_timeStamp < cl.timeStamp)
                    {
                        m_timeStamp = cl.timeStamp;
                    }
                }
            }
        }

        void dumpPopulation(std::ostream & os) const
        {
            os << "Condition,int,prediction,epsilon,F,exp,ts,as,n,acc\n";
            for (const auto & cl : m_population)
            {
                os  << cl->condition << ","
                    << cl->action << ","
                    << cl->prediction << ","
                    << cl->epsilon << ","
                    << cl->fitness << ","
                    << cl->experience << ","
                    << cl->timeStamp << ","
                    << cl->actionSetSize << ","
                    << cl->numerosity << ","
                    << cl->accuracy() << "\n";
            }
        }

        std::size_t populationSize() const
        {
            return m_population.size();
        }

        std::size_t numerositySum() const
        {
            std::uint64_t sum = 0;
            for (const auto & cl : m_population)
            {
                sum += cl->numerosity;
            }
            return sum;
        }

        void switchToCondensationMode()
        {
            m_constants.chi = 0.0;
            m_constants.mu = 0.0;
        }
    };

}
