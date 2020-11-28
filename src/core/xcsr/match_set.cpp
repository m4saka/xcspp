#include "xcspp/core/xcsr/match_set.hpp"
#include <memory> // std::make_shared
#include <sstream> // std::ostringstream

namespace xcspp::xcsr
{

    namespace
    {
        // GENERATE COVERING CLASSIFIER
        ClassifierPtr GenerateCoveringClassifier(
            const std::vector<double> & situation,
            const std::unordered_set<int> & unselectedActions,
            std::uint64_t timeStamp,
            const XCSRParams *pParams,
            Random & random)
        {
            const auto cl = std::make_shared<StoredClassifier>(situation, random.chooseFrom(unselectedActions), timeStamp, pParams);

            // Set to "#" (don't care) at random
            for (auto & symbol : cl->condition)
            {
                if (random.nextDouble() < pParams->dontCareProbability)
                {
                    symbol.setToDontCare();
                }
            }

            return cl;
        }
    }

    MatchSet::MatchSet(Population & population, const std::vector<double> & situation, std::uint64_t timeStamp, const XCSRParams *pParams, const std::unordered_set<int> & availableActions, Random & random)
        : ClassifierPtrSet(pParams, availableActions)
        , m_isCoveringPerformed(false)
    {
        generateSet(population, situation, timeStamp, random);
    }

    // GENERATE MATCH SET
    void MatchSet::generateSet(Population & population, const std::vector<double> & situation, std::uint64_t timeStamp, Random & random)
    {
        // Set theta_mna (the minimal number of actions) to the number of action choices if theta_mna is 0
        auto thetaMna = (m_pParams->thetaMna == 0) ? m_availableActions.size() : m_pParams->thetaMna;

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
                const auto coveringClassifier = GenerateCoveringClassifier(situation, unselectedActions, timeStamp, m_pParams, random);

                // Make sure the generated covering classifier covers the given input
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
                population.deleteExtraClassifiers(random);
                m_set.clear();
                m_isCoveringPerformed = true;
            }
            else
            {
                m_isCoveringPerformed = false;
            }
        }
    }

    bool MatchSet::isCoveringPerformed() const
    {
        return m_isCoveringPerformed;
    }

}
