#pragma once
#include "classifier_ptr_set.hpp"
#include "random.hpp"

namespace xcscpp
{

    class Population : public ClassifierPtrSet
    {
    protected:
        // DELETION VOTE
        virtual double deletionVote(const Classifier & cl, double averageFitness) const
        {
            double vote = cl.actionSetSize * cl.numerosity;

            // Consider fitness for deletion vote
            if ((cl.experience >= m_pConstants->thetaDel) && (cl.fitness / cl.numerosity < m_pConstants->delta * averageFitness))
            {
                vote *= averageFitness / (cl.fitness / cl.numerosity);
            }

            return vote;
        }

    public:
        // Constructor
        using ClassifierPtrSet::ClassifierPtrSet;

        // Destructor
        virtual ~Population() = default;

        // INSERT IN POPULATION
        virtual void insertOrIncrementNumerosity(const ClassifierPtr & cl)
        {
            for (auto & c : m_set)
            {
                if (c->condition == cl->condition && c->action == cl->action)
                {
                    ++c->numerosity;
                    return;
                }
            }
            m_set.insert(cl);
        }

        // DELETE FROM POPULATION
        virtual bool deleteExtraClassifiers()
        {
            uint64_t numerositySum = 0;
            double fitnessSum = 0.0;
            for (const auto & c : m_set)
            {
                numerositySum += c->numerosity;
                fitnessSum += c->fitness;
            }

            // Return false if the sum of numerosity has not met its maximum limit
            if (numerositySum <= m_pConstants->n)
            {
                return false;
            }

            // The average fitness in the population
            double averageFitness = fitnessSum / numerositySum;

            std::vector<const ClassifierPtr *> targets;
            for (const auto & cl : m_set)
            {
                targets.push_back(&cl);
            }

            // Roulette-wheel selection
            std::vector<double> votes;
            votes.reserve(targets.size());
            for (const auto & target : targets)
            {
                votes.push_back(deletionVote(**target, averageFitness));
            }
            std::size_t selectedIdx = Random::rouletteWheelSelection(votes);

            // Distrust the selected classifier
            if ((*targets[selectedIdx])->numerosity > 1)
            {
                (*targets[selectedIdx])->numerosity--;
            }
            else
            {
                m_set.erase(*targets[selectedIdx]);
            }

            return (numerositySum - 1) > m_pConstants->n;
        }
    };

}
