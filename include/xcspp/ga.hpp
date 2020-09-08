#pragma once
#include <memory> // std::shared_ptr, std::make_shared
#include <vector>
#include <unordered_set>
#include <cstdint> // std::uint64_t
#include <cstddef> // std::size_t

#include "classifier_ptr_set.hpp"

namespace xcspp
{

    // TODO: use namespace instead of class
    //       (In legacy xxr, GA was implemented as a class in order to use inheritance for different algorithms)
    class GA
    {
    private:
        const XCSParams * const m_pParams;
        const std::unordered_set<int> m_availableActions;

        // SELECT OFFSPRING
        ClassifierPtr selectOffspring(const ClassifierPtrSet & actionSet) const
        {
            std::vector<const ClassifierPtr *> targets;
            for (const auto & cl : actionSet)
            {
                targets.push_back(&cl);
            }

            std::size_t selectedIdx;
            if (m_pParams->tau > 0.0 && m_pParams->tau <= 1.0)
            {
                // Tournament selection
                std::vector<std::pair<double, std::uint64_t>> fitnesses;
                fitnesses.reserve(actionSet.size());
                for (const auto & target : targets)
                {
                    fitnesses.emplace_back((*target)->fitness, (*target)->numerosity);
                }
                selectedIdx = m_pParams->random.tournamentSelectionMicroClassifier(fitnesses, m_pParams->tau);
            }
            else
            {
                // Roulette-wheel selection
                std::vector<double> fitnesses;
                fitnesses.reserve(actionSet.size());
                for (const auto & target : targets)
                {
                    fitnesses.push_back((*target)->fitness);
                }
                selectedIdx = m_pParams->random.rouletteWheelSelection(fitnesses);
            }
            return *targets[selectedIdx];
        }

        // APPLY CROSSOVER (uniform crossover)
        bool uniformCrossover(Classifier & cl1, Classifier & cl2) const
        {
            if (cl1.condition.size() != cl2.condition.size())
            {
                throw std::invalid_argument("The condition lengths do not match in GA::uniformCrossover().");
            }

            bool isChanged = false;
            for (std::size_t i = 0; i < cl1.condition.size(); ++i)
            {
                if (m_pParams->random.nextDouble() < 0.5)
                {
                    std::swap(cl1.condition[i], cl2.condition[i]);
                    isChanged = true;
                }
            }
            return isChanged;
        }

        // APPLY CROSSOVER (one point crossover)
        bool onePointCrossover(Classifier & cl1, Classifier & cl2) const
        {
            if (cl1.condition.size() != cl2.condition.size())
            {
                throw std::invalid_argument("The condition lengths do not match in GA::onePointCrossover().");
            }

            std::size_t x = m_pParams->random.nextInt<std::size_t>(0, cl1.condition.size());

            bool isChanged = false;
            for (std::size_t i = x + 1; i < cl1.condition.size(); ++i)
            {
                std::swap(cl1.condition[i], cl2.condition[i]);
                isChanged = true;
            }
            return isChanged;
        }

        // APPLY CROSSOVER (two point crossover)
        bool twoPointCrossover(Classifier & cl1, Classifier & cl2) const
        {
            if (cl1.condition.size() != cl2.condition.size())
            {
                throw std::invalid_argument("The condition lengths do not match in GA::twoPointCrossover().");
            }

            std::size_t x = m_pParams->random.nextInt<std::size_t>(0, cl1.condition.size());
            std::size_t y = m_pParams->random.nextInt<std::size_t>(0, cl1.condition.size());

            if (x > y)
            {
                std::swap(x, y);
            }

            bool isChanged = false;
            for (std::size_t i = x + 1; i < y; ++i)
            {
                std::swap(cl1.condition[i], cl2.condition[i]);
                isChanged = true;
            }
            return isChanged;
        }

        // APPLY CROSSOVER
        bool crossover(Classifier & cl1, Classifier & cl2) const
        {
            switch (m_pParams->crossoverMethod)
            {
            case XCSParams::CrossoverMethod::UNIFORM_CROSSOVER:
                return uniformCrossover(cl1, cl2);

            case XCSParams::CrossoverMethod::ONE_POINT_CROSSOVER:
                return onePointCrossover(cl1, cl2);

            case XCSParams::CrossoverMethod::TWO_POINT_CROSSOVER:
                return twoPointCrossover(cl1, cl2);

            default:
                return false;
            }
        }

        // APPLY MUTATION
        void mutate(Classifier & cl, const std::vector<int> & situation) const
        {
            if (cl.condition.size() != situation.size())
            {
                std::invalid_argument("GA::mutate() could not process the situation with a different length.");
            }

            for (std::size_t i = 0; i < cl.condition.size(); ++i)
            {
                if (m_pParams->random.nextDouble() < m_pParams->mu)
                {
                    if (cl.condition[i].isDontCare())
                    {
                        cl.condition[i] = Symbol(situation.at(i));
                    }
                    else
                    {
                        cl.condition[i].setToDontCare();
                    }
                }
            }

            if (m_pParams->doActionMutation && (m_pParams->random.nextDouble() < m_pParams->mu) && (m_availableActions.size() >= 2))
            {
                std::unordered_set<int> otherPossibleActions(m_availableActions);
                otherPossibleActions.erase(cl.action);
                cl.action = m_pParams->random.chooseFrom(otherPossibleActions);
            }
        }

        void insertDiscoveredClassifiers(const Classifier & child1, const Classifier & child2, const ClassifierPtr & parent1, const ClassifierPtr & parent2, Population & population) const
        {
            if (m_pParams->doGASubsumption)
            {
                subsumeClassifier(child1, parent1, parent2, population);
                subsumeClassifier(child2, parent1, parent2, population);
            }
            else
            {
                population.insertOrIncrementNumerosity(std::make_shared<StoredClassifier>(child1, m_pParams));
                population.insertOrIncrementNumerosity(std::make_shared<StoredClassifier>(child2, m_pParams));
            }

            while (population.deleteExtraClassifiers()) {}
        }

        void subsumeClassifier(const Classifier & child, const ClassifierPtr & parent1, const ClassifierPtr & parent2, Population & population) const
        {
            if (parent1->subsumes(child))
            {
                ++parent1->numerosity;
            }
            else if (parent2->subsumes(child))
            {
                ++parent2->numerosity;
            }
            else
            {
                subsumeClassifier(child, population); // calls second subsumeClassifier function!
            }
        }

        void subsumeClassifier(const Classifier & child, Population & population) const
        {
            std::vector<ClassifierPtr> choices;

            for (const auto & cl : population)
            {
                if (cl->subsumes(child))
                {
                    choices.push_back(cl);
                }
            }

            if (!choices.empty())
            {
                std::size_t choice = m_pParams->random.nextInt<std::size_t>(0, choices.size() - 1);
                ++choices[choice]->numerosity;
                return;
            }

            population.insertOrIncrementNumerosity(std::make_shared<StoredClassifier>(child, m_pParams));
        }

    public:
        // Constructor
        GA(const XCSParams *pParams, const std::unordered_set<int> & availableActions)
            : m_pParams(pParams)
            , m_availableActions(availableActions)
        {
        }

        // Destructor
        ~GA() = default;

        // RUN GA (refer to ActionSet::runGA() for the former part)
        void run(ClassifierPtrSet & actionSet, const std::vector<int> & situation, Population & population) const
        {
            const ClassifierPtr parent1 = selectOffspring(actionSet);
            const ClassifierPtr parent2 = selectOffspring(actionSet);
            if (parent1->condition.size() != parent2->condition.size())
            {
                std::domain_error("The condition lengths of selected parents do not match in GA::run().");
            }

            Classifier child1(*parent1);
            Classifier child2(*parent2);
            child1.fitness = parent1->fitness / parent1->numerosity;
            child2.fitness = parent2->fitness / parent2->numerosity;
            child1.numerosity = child2.numerosity = 1;
            child1.experience = child2.experience = 0;

            bool isChangedByCrossover;
            if (m_pParams->random.nextDouble() < m_pParams->chi)
            {
                isChangedByCrossover = crossover(child1, child2);
            }
            else
            {
                isChangedByCrossover = false;
            }

            mutate(child1, situation);
            mutate(child2, situation);

            if (isChangedByCrossover)
            {
                child1.prediction =
                    child2.prediction = (child1.prediction + child2.prediction) / 2;

                child1.epsilon =
                    child2.epsilon = (child1.epsilon + child2.epsilon) / 2;

                child1.fitness =
                    child2.fitness = (child1.fitness + child2.fitness) / 2 * 0.1; // fitnessReduction
            }
            else
            {
                child1.fitness *= 0.1; // fitnessReduction
                child2.fitness *= 0.1; // fitnessReduction
            }

            insertDiscoveredClassifiers(child1, child2, parent1, parent2, population);
        }
    };

}
