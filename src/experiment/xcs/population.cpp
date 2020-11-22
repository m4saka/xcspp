#include "xcspp/experiment/xcs/population.hpp"
#include <cstdint> // std::uint64_t

#include "xcspp/experiment/xcs/classifier_ptr_set.hpp"
#include "xcspp/util/random.hpp"

namespace xcspp
{

    namespace
    {
        // DELETION VOTE
        double DeletionVote(const Classifier & cl, double averageFitness, std::uint64_t thetaDel, double delta)
        {
            double vote = cl.actionSetSize * cl.numerosity;

            // Consider fitness for deletion vote
            if ((cl.experience >= thetaDel) && (cl.fitness / cl.numerosity < delta * averageFitness))
            {
                vote *= averageFitness / (cl.fitness / cl.numerosity);
            }

            return vote;
        }
    }

    // INSERT IN POPULATION
    void Population::insertOrIncrementNumerosity(const ClassifierPtr & cl)
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
    bool Population::deleteExtraClassifiers(Random & random)
    {
        uint64_t numerositySum = 0;
        double fitnessSum = 0.0;
        for (const auto & c : m_set)
        {
            numerositySum += c->numerosity;
            fitnessSum += c->fitness;
        }

        // Return false if the sum of numerosity has not met its maximum limit
        if (numerositySum <= m_pParams->n)
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
            votes.push_back(DeletionVote(**target, averageFitness, m_pParams->thetaDel, m_pParams->delta));
        }
        std::size_t selectedIdx = random.rouletteWheelSelection(votes);

        // Distrust the selected classifier
        if ((*targets[selectedIdx])->numerosity > 1)
        {
            (*targets[selectedIdx])->numerosity--;
        }
        else
        {
            m_set.erase(*targets[selectedIdx]);
        }

        return (numerositySum - 1) > m_pParams->n;
    }

}
