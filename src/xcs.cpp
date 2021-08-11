#include "xcspp/xcs/xcs.hpp"
#include <iostream>
#include <memory> // std::make_shared

#include "xcspp/xcs/match_set.hpp"
#include "xcspp/util/csv.hpp"

namespace xcspp
{

    void XCS::syncTimeStampWithPopulation()
    {
        m_timeStamp = 0;
        for (const auto & cl : m_population)
        {
            if (m_timeStamp < cl->timeStamp)
            {
                m_timeStamp = cl->timeStamp;
            }
        }
    }

    XCS::XCS(const std::unordered_set<int> & availableActions, const XCSParams & params)
        : m_params(params)
        , m_population(&m_params, availableActions)
        , m_actionSet(&m_params, availableActions)
        , m_prevActionSet(&m_params, availableActions)
        , m_availableActions(availableActions)
        , m_timeStamp(0)
        , m_expectsReward(false)
        , m_prevReward(0.0)
        , m_isPrevModeExplore(false)
        , m_prediction(0.0)
        , m_isCoveringPerformed(false)
    {
    }

    int XCS::explore(const std::vector<int> & situation)
    {
        if (m_expectsReward)
        {
            throw std::domain_error("XCS::explore() is called although XCS expects reward() to be called.");
        }

        // [M]
        //   The match set [M] is formed out of the current [P].
        //   It includes all classifiers that match the current situation.
        const MatchSet matchSet(m_population, situation, m_timeStamp, &m_params, m_availableActions, m_random);
        m_isCoveringPerformed = matchSet.isCoveringPerformed();

        const PredictionArray predictionArray(matchSet, &m_params);

        const int action = predictionArray.selectAction(m_params.exploreProbability, m_random);
        m_prediction = predictionArray.predictionFor(action);
        for (const auto & a : m_availableActions)
        {
            m_predictions[a] = predictionArray.predictionFor(a);
        }

        m_actionSet.generateSet(matchSet, action);

        m_expectsReward = true;
        m_isPrevModeExplore = true;

        if (!m_prevActionSet.empty())
        {
            double p = m_prevReward + m_params.gamma * predictionArray.max();
            m_prevActionSet.update(p, m_population);
            m_prevActionSet.runGA(m_prevSituation, m_population, m_timeStamp, m_random);
        }

        m_prevSituation = situation;

        return action;
    }

    void XCS::reward(double value, bool isEndOfProblem)
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
                m_actionSet.runGA(m_prevSituation, m_population, m_timeStamp, m_random);
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

    int XCS::exploit(const std::vector<int> & situation, bool update)
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
            const MatchSet matchSet(m_population, situation, m_timeStamp, &m_params, m_availableActions, m_random);
            m_isCoveringPerformed = matchSet.isCoveringPerformed();

            const PredictionArray predictionArray(matchSet, &m_params);

            const int action = predictionArray.selectAction(0.0, m_random);

            m_actionSet.generateSet(matchSet, action);

            m_expectsReward = true;
            m_isPrevModeExplore = false;

            if (!m_prevActionSet.empty())
            {
                double p = m_prevReward + m_params.gamma * predictionArray.max();
                m_prevActionSet.update(p, m_population);

                // Do not perform GA operations in exploitation
            }

            m_prevSituation = situation;

            return action;
        }
        else
        {
            // Create new match set as sandbox
            MatchSet matchSet(&m_params, m_availableActions);
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

                PredictionArray predictionArray(matchSet, &m_params);
                const int action = predictionArray.selectAction(0.0, m_random);
                m_prediction = predictionArray.predictionFor(action);
                for (const auto & a : m_availableActions)
                {
                    m_predictions[a] = predictionArray.predictionFor(a);
                }
                return action;
            }
            else
            {
                m_isCoveringPerformed = true;
                m_prediction = m_params.initialPrediction;
                for (const auto & action : m_availableActions)
                {
                    m_predictions[action] = m_params.initialPrediction;
                }
                return m_random.chooseFrom(m_availableActions);
            }
        }
    }

    double XCS::prediction() const
    {
        return m_prediction;
    }

    double XCS::predictionFor(int action) const
    {
        return m_predictions.at(action);
    }

    bool XCS::isCoveringPerformed() const
    {
        return m_isCoveringPerformed;
    }

    std::vector<Classifier> XCS::getMatchingClassifiers(const std::vector<int> & situation) const
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

    const Population & XCS::population() const
    {
        return m_population;
    }

    void XCS::setPopulationClassifiers(const std::vector<Classifier> & classifiers, bool syncTimeStamp)
    {
        m_population.setClassifiers(classifiers);

        // Set system timestamp to the same as latest classifier
        if (syncTimeStamp)
        {
            syncTimeStampWithPopulation();
        }

        // Clear action set and reset status
        m_actionSet.clear();
        m_prevActionSet.clear();
        m_expectsReward = false;
        m_isPrevModeExplore = false;
    }

    // deprecated
    void XCS::dumpPopulation(std::ostream & os) const
    {
        m_population.outputCSV(os);
    }

    void XCS::outputPopulationCSV(std::ostream & os) const
    {
        m_population.outputCSV(os);
    }

    bool XCS::loadPopulationCSVFile(const std::string & filename, bool initClassifierVariables, bool syncTimeStamp)
    {
        bool ret = m_population.loadCSVFile(filename, initClassifierVariables);

        // Set system timestamp to the same as latest classifier
        if (syncTimeStamp)
        {
            syncTimeStampWithPopulation();
        }

        // Clear action set and reset status
        m_actionSet.clear();
        m_prevActionSet.clear();
        m_expectsReward = false;
        m_isPrevModeExplore = false;

        return ret;
    }

    bool XCS::savePopulationCSVFile(const std::string & filename)
    {
        return m_population.saveCSVFile(filename);
    }

    std::size_t XCS::populationSize() const
    {
        return m_population.size();
    }

    std::size_t XCS::numerositySum() const
    {
        std::uint64_t sum = 0;
        for (const auto & cl : m_population)
        {
            sum += cl->numerosity;
        }
        return sum;
    }

    void XCS::switchToCondensationMode()
    {
        m_params.chi = 0.0;
        m_params.mu = 0.0;
    }

}
