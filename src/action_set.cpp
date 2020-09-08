#include "xcspp/action_set.hpp"
#include <cmath>

namespace xcspp
{

    // UPDATE FITNESS
    void ActionSet::updateFitness()
    {
        double accuracySum = 0.0;
        for (const auto & cl : m_set)
        {
            accuracySum += cl->accuracy() * cl->numerosity;
        }

        for (const auto & cl : m_set)
        {
            cl->fitness += m_pParams->beta * (cl->accuracy() * cl->numerosity / accuracySum - cl->fitness);
        }
    }

    // DO ACTION SET SUBSUMPTION
    void ActionSet::doSubsumption(Population & population)
    {
        ClassifierPtr cl;
        for (const auto & c : m_set)
        {
            if (c->isSubsumer())
            {
                if ((cl.get() == nullptr) || c->isMoreGeneral(*cl))
                {
                    cl = c;
                }
            }
        }

        if (cl.get() != nullptr)
        {
            std::vector<ClassifierPtr> removedClassifiers;
            for (const auto & c : m_set)
            {
                if (cl->isMoreGeneral(*c))
                {
                    cl->numerosity += c->numerosity;
                    removedClassifiers.push_back(c);
                }
            }

            for (const auto & removedClassifier : removedClassifiers)
            {
                population.erase(removedClassifier);
                m_set.erase(removedClassifier);
            }
        }
    }

    ActionSet::ActionSet(const XCSParams *pParams, const std::unordered_set<int> & availableActions)
        : ClassifierPtrSet(pParams, availableActions)
        , m_ga(pParams, availableActions)
    {
    }

    ActionSet::ActionSet(const MatchSet & matchSet, int action, const XCSParams *pParams, const std::unordered_set<int> & availableActions)
        : ClassifierPtrSet(pParams, availableActions)
        , m_ga(pParams, availableActions)
    {
        generateSet(matchSet, action);
    }

    // GENERATE ACTION SET
    void ActionSet::generateSet(const MatchSet & matchSet, int action)
    {
        m_set.clear();

        for (const auto & cl : matchSet)
        {
            if (cl->action == action)
            {
                m_set.insert(cl);
            }
        }
    }

    void ActionSet::copyTo(ActionSet & dest)
    {
        dest.m_set = m_set; // do not copy m_ga since it contains const parameters
    }

    // RUN GA (refer to GA::run() for the latter part)
    void ActionSet::runGA(const std::vector<int> & situation, Population & population, std::uint64_t timeStamp, Random & random)
    {
        double numerositySum = 0.0;
        for (const auto & cl : m_set)
        {
            numerositySum += cl->numerosity;
        }
        if (numerositySum <= 0.0)
        {
            throw std::runtime_error("Invalid numerosity sum detected in ActionSet::runGA().");
        }

        double averageTimeStamp = 0.0;
        for (const auto & cl : m_set)
        {
            averageTimeStamp += cl->timeStamp / numerositySum * cl->numerosity;
        }
        if (averageTimeStamp >= timeStamp + 1)
        {
            throw std::runtime_error("Invalid average timestamp detected in ActionSet::runGA().");
        }

        if (timeStamp - averageTimeStamp >= m_pParams->thetaGA)
        {
            for (const auto & cl : m_set)
            {
                cl->timeStamp = timeStamp;
            }

            m_ga.run(*this, situation, population, random);
        }
    }

    // UPDATE SET
    void ActionSet::update(double p, Population & population)
    {
        // Calculate numerosity sum used for updating action set size estimate
        std::uint64_t numerositySum = 0;
        for (const auto & cl : m_set)
        {
            numerositySum += cl->numerosity;
        }

        for (const auto & cl : m_set)
        {
            ++cl->experience;

            // Update prediction, prediction error
            if (m_pParams->useMAM && cl->experience < 1.0 / m_pParams->beta)
            {
                cl->epsilon += (std::abs(p - cl->prediction) - cl->epsilon) / cl->experience;
                cl->prediction += (p - cl->prediction) / cl->experience;
            }
            else
            {
                cl->epsilon += m_pParams->beta * (std::abs(p - cl->prediction) - cl->epsilon);
                cl->prediction += m_pParams->beta * (p - cl->prediction);
            }

            // Update action set size estimate
            if (cl->experience < 1.0 / m_pParams->beta)
            {
                cl->actionSetSize += (numerositySum - cl->actionSetSize) / cl->experience;
            }
            else
            {
                cl->actionSetSize += m_pParams->beta * (numerositySum - cl->actionSetSize);
            }
        }

        updateFitness();

        if (m_pParams->doActionSetSubsumption)
        {
            doSubsumption(population);
        }
    }

}
