#pragma once
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cstdint>
#include <cmath>

#include "classifier_ptr_set.hpp"
#include "population.hpp"
#include "match_set.hpp"
#include "ga.hpp"
#include "constants.hpp"

namespace xcscpp
{

    class ActionSet : public ClassifierPtrSet
    {
    protected:
        GA m_ga;

        // UPDATE FITNESS
        void updateFitness()
        {
            double accuracySum = 0.0;
            for (const auto & cl : m_set)
            {
                accuracySum += cl->accuracy() * cl->numerosity;
            }

            for (const auto & cl : m_set)
            {
                cl->fitness += m_pConstants->beta * (cl->accuracy() * cl->numerosity / accuracySum - cl->fitness);
            }
        }

        // DO ACTION SET SUBSUMPTION
        void doSubsumption(Population & population)
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

    public:
        // Constructor
        ActionSet(Constants *pConstants, const std::unordered_set<int> & availableActions)
            : ClassifierPtrSet(pConstants, availableActions)
            , m_ga(pConstants, availableActions)
        {
        }

        ActionSet(const MatchSet & matchSet, int action, Constants *pConstants, const std::unordered_set<int> & availableActions)
            : ClassifierPtrSet(pConstants, availableActions)
            , m_ga(pConstants, availableActions)
        {
            regenerate(matchSet, action);
        }

        // Destructor
        virtual ~ActionSet() = default;

        // GENERATE ACTION SET
        void regenerate(const MatchSet & matchSet, int action)
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

        void copyTo(ActionSet & dest)
        {
            dest.m_set = m_set; // don't copy m_ga since it contains const parameters
        }

        // RUN GA (refer to GA::run() for the latter part)
        void runGA(const std::vector<int> & situation, Population & population, std::uint64_t timeStamp)
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

            if (timeStamp - averageTimeStamp >= m_pConstants->thetaGA)
            {
                for (const auto & cl : m_set)
                {
                    cl->timeStamp = timeStamp;
                }

                m_ga.run(*this, situation, population);
            }
        }

        // UPDATE SET
        void update(double p, Population & population)
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
                if (m_pConstants->useMAM && cl->experience < 1.0 / m_pConstants->beta)
                {
                    cl->epsilon += (std::abs(p - cl->prediction) - cl->epsilon) / cl->experience;
                    cl->prediction += (p - cl->prediction) / cl->experience;
                }
                else
                {
                    cl->epsilon += m_pConstants->beta * (std::abs(p - cl->prediction) - cl->epsilon);
                    cl->prediction += m_pConstants->beta * (p - cl->prediction);
                }

                // Update action set size estimate
                if (cl->experience < 1.0 / m_pConstants->beta)
                {
                    cl->actionSetSize += (numerositySum - cl->actionSetSize) / cl->experience;
                }
                else
                {
                    cl->actionSetSize += m_pConstants->beta * (numerositySum - cl->actionSetSize);
                }
            }

            updateFitness();

            if (m_pConstants->doActionSetSubsumption)
            {
                doSubsumption(population);
            }
        }
    };

}
