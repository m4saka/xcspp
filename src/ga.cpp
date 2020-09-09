#include "xcspp/ga.hpp"
#include <memory> // std::shared_ptr, std::make_shared
#include <vector>
#include <unordered_set>
#include <cstdint> // std::uint64_t
#include <cstddef> // std::size_t

namespace xcspp
{

    namespace
    {
        // SELECT OFFSPRING
        ClassifierPtr selectOffspring(const ClassifierPtrSet & actionSet, double tau, Random & random)
        {
            std::vector<const ClassifierPtr *> targets;
            for (const auto & cl : actionSet)
            {
                targets.push_back(&cl);
            }

            std::size_t selectedIdx;
            if (tau > 0.0 && tau <= 1.0)
            {
                // Tournament selection
                std::vector<std::pair<double, std::uint64_t>> fitnesses;
                fitnesses.reserve(actionSet.size());
                for (const auto & target : targets)
                {
                    fitnesses.emplace_back((*target)->fitness, (*target)->numerosity);
                }
                selectedIdx = random.tournamentSelectionMicroClassifier(fitnesses, tau);
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
                selectedIdx = random.rouletteWheelSelection(fitnesses);
            }
            return *targets[selectedIdx];
        }

        // APPLY CROSSOVER (uniform crossover)
        bool uniformCrossover(Classifier & cl1, Classifier & cl2, Random & random)
        {
            if (cl1.condition.size() != cl2.condition.size())
            {
                throw std::invalid_argument("The condition lengths do not match in GA::uniformCrossover().");
            }

            bool isChanged = false;
            for (std::size_t i = 0; i < cl1.condition.size(); ++i)
            {
                if (random.nextDouble() < 0.5)
                {
                    std::swap(cl1.condition[i], cl2.condition[i]);
                    isChanged = true;
                }
            }
            return isChanged;
        }

        // APPLY CROSSOVER (one point crossover)
        bool onePointCrossover(Classifier & cl1, Classifier & cl2, Random & random)
        {
            if (cl1.condition.size() != cl2.condition.size())
            {
                throw std::invalid_argument("The condition lengths do not match in GA::onePointCrossover().");
            }

            std::size_t x = random.nextInt<std::size_t>(0, cl1.condition.size());

            bool isChanged = false;
            for (std::size_t i = x + 1; i < cl1.condition.size(); ++i)
            {
                std::swap(cl1.condition[i], cl2.condition[i]);
                isChanged = true;
            }
            return isChanged;
        }

        // APPLY CROSSOVER (two point crossover)
        bool twoPointCrossover(Classifier & cl1, Classifier & cl2, Random & random)
        {
            if (cl1.condition.size() != cl2.condition.size())
            {
                throw std::invalid_argument("The condition lengths do not match in GA::twoPointCrossover().");
            }

            std::size_t x = random.nextInt<std::size_t>(0, cl1.condition.size());
            std::size_t y = random.nextInt<std::size_t>(0, cl1.condition.size());

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
        bool crossover(Classifier & cl1, Classifier & cl2, XCSParams::CrossoverMethod crossoverMethod, Random & random)
        {
            switch (crossoverMethod)
            {
            case XCSParams::CrossoverMethod::UNIFORM_CROSSOVER:
                return uniformCrossover(cl1, cl2, random);

            case XCSParams::CrossoverMethod::ONE_POINT_CROSSOVER:
                return onePointCrossover(cl1, cl2, random);

            case XCSParams::CrossoverMethod::TWO_POINT_CROSSOVER:
                return twoPointCrossover(cl1, cl2, random);

            default:
                return false;
            }
        }

        // APPLY MUTATION
        void mutate(Classifier & cl, const std::vector<int> & situation, const std::unordered_set<int> & availableActions, double mu, bool doActionMutation, Random & random)
        {
            if (cl.condition.size() != situation.size())
            {
                std::invalid_argument("GA::mutate() could not process the situation with a different length.");
            }

            for (std::size_t i = 0; i < cl.condition.size(); ++i)
            {
                if (random.nextDouble() < mu)
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

            if (doActionMutation && (random.nextDouble() < mu) && (availableActions.size() >= 2))
            {
                std::unordered_set<int> otherPossibleActions(availableActions);
                otherPossibleActions.erase(cl.action);
                cl.action = random.chooseFrom(otherPossibleActions);
            }
        }

        void subsumeClassifier(const Classifier & child, Population & population, const XCSParams *pParams, Random & random)
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
                std::size_t choice = random.nextInt<std::size_t>(0, choices.size() - 1);
                ++choices[choice]->numerosity;
                return;
            }

            population.insertOrIncrementNumerosity(std::make_shared<StoredClassifier>(child, pParams));
        }

        void subsumeClassifier(const Classifier & child, const ClassifierPtr & parent1, const ClassifierPtr & parent2, Population & population, const XCSParams *pParams, Random & random)
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
                subsumeClassifier(child, population, pParams, random); // calls first subsumeClassifier function!
            }
        }

        void insertDiscoveredClassifiers(const Classifier & child1, const Classifier & child2, const ClassifierPtr & parent1, const ClassifierPtr & parent2, Population & population, const XCSParams *pParams, Random & random)
        {
            if (pParams->doGASubsumption)
            {
                subsumeClassifier(child1, parent1, parent2, population, pParams, random);
                subsumeClassifier(child2, parent1, parent2, population, pParams, random);
            }
            else
            {
                population.insertOrIncrementNumerosity(std::make_shared<StoredClassifier>(child1, pParams));
                population.insertOrIncrementNumerosity(std::make_shared<StoredClassifier>(child2, pParams));
            }

            while (population.deleteExtraClassifiers(random)) {}
        }
    }

    namespace GA
    {
        // RUN GA (refer to ActionSet::runGA() for the former part)
        void run(ClassifierPtrSet & actionSet, const std::vector<int> & situation, Population & population, const std::unordered_set<int> & availableActions, const XCSParams *pParams, Random & random)
        {
            const ClassifierPtr parent1 = selectOffspring(actionSet, pParams->tau, random);
            const ClassifierPtr parent2 = selectOffspring(actionSet, pParams->tau, random);
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
            if (random.nextDouble() < pParams->chi)
            {
                isChangedByCrossover = crossover(child1, child2, pParams->crossoverMethod, random);
            }
            else
            {
                isChangedByCrossover = false;
            }

            mutate(child1, situation, availableActions, pParams->mu, pParams->doActionMutation, random);
            mutate(child2, situation, availableActions, pParams->mu, pParams->doActionMutation, random);

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

            insertDiscoveredClassifiers(child1, child2, parent1, parent2, population, pParams, random);
        }
    }

}
