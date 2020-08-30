#pragma once
#include <memory> // std::make_shared
#include <unordered_map>
#include <cstdint> // std::uint64_t
#include <sstream> // std::ostringstream

#include "classifier_ptr_set.hpp"
#include "population.hpp"
#include "constants.hpp"
#include "random.hpp"

namespace xcscpp
{

    class MatchSet : public ClassifierPtrSet
    {
    protected:
        bool m_isCoveringPerformed;

        // GENERATE COVERING CLASSIFIER
        ClassifierPtr generateCoveringClassifier(const std::vector<int> & situation, const std::unordered_set<int> & unselectedActions, std::uint64_t timeStamp) const
        {
            const auto cl = std::make_shared<StoredClassifier>(situation, Random::chooseFrom(unselectedActions), timeStamp, m_pConstants);
            cl->condition.setToDontCareAtRandom(m_pConstants->dontCareProbability);

            return cl;
        }

    public:
        // Constructor
        using ClassifierPtrSet::ClassifierPtrSet;

        MatchSet(Population & population, const std::vector<int> & situation, std::uint64_t timeStamp, const Constants *pConstants, const std::unordered_set<int> & availableActions)
            : ClassifierPtrSet(pConstants, availableActions)
            , m_isCoveringPerformed(false)
        {
            regenerate(population, situation, timeStamp);
        }

        // Destructor
        virtual ~MatchSet() = default;

        // GENERATE MATCH SET
        void regenerate(Population & population, const std::vector<int> & situation, std::uint64_t timeStamp)
        {
            // Set theta_mna (the minimal number of actions) to the number of action choices if theta_mna is 0
            auto thetaMna = (m_pConstants->thetaMna == 0) ? m_availableActions.size() : m_pConstants->thetaMna;

            auto unselectedActions = m_availableActions;

            m_set.clear();

            while (m_set.empty())
            {
                for (const auto & cl : population)
                {
                    if (cl->condition.matches(situation))
                    {
                        m_set.insert(cl);
                        unselectedActions.erase(cl->action);
                    }
                }

                // Generate classifiers covering the unselected actions
                if (m_availableActions.size() - unselectedActions.size() < thetaMna)
                {
                    const auto coveringClassifier = generateCoveringClassifier(situation, unselectedActions, timeStamp);
                    if (!coveringClassifier->condition.matches(situation))
                    {
                        std::ostringstream oss;
                        oss <<
                            "The covering classifier does not contain the current situation!\n"
                            "  - Current situation: ";
                        for (const auto & s : situation)
                        {
                            oss << s << ' ';
                        }
                        oss << "\n  - Covering classifier: " << *coveringClassifier << '\n' << std::endl;
                        throw std::runtime_error(oss.str());
                    }
                    population.insert(coveringClassifier);
                    population.deleteExtraClassifiers();
                    m_set.clear();
                    m_isCoveringPerformed = true;
                }
                else
                {
                    m_isCoveringPerformed = false;
                }
            }
        }

        // Get if covering is performed in the previous match set generation
        // (Call this function after constructor or regenerate())
        bool isCoveringPerformed() const
        {
            return m_isCoveringPerformed;
        }
    };

}
