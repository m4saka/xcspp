#include "xcspp/xcs/prediction_array.hpp"
#include <cfloat> // DBL_EPSILON
#include <cmath> // std::abs

#include "xcspp/util/random.hpp"

namespace xcspp
{

    namespace
    {
        // This is non-zero since reward can have negative values
        constexpr double kInitialMaxPA = -100000.0;
    }

    // GENERATE PREDICTION ARRAY
    PredictionArray::PredictionArray(const MatchSet & matchSet, const XCSParams *pParams)
        : m_pParams(pParams)
    {
        // FSA (Fitness Sum Array)
        std::unordered_map<int, double> fsa;

        for (const auto & cl : matchSet)
        {
            if (m_pa.count(cl->action) == 0) {
                m_paActions.push_back(cl->action);
            }

            // Note: it is okay to skip zero initialization before these
            //       because std::unordered_map::operator[] does zero initialization.
            m_pa[cl->action] += cl->prediction * cl->fitness;
            fsa[cl->action] += cl->fitness;
        }

        m_maxPA = kInitialMaxPA;

        for (auto & [ action, prediction ] : m_pa)
        {
            if (std::abs(fsa[action]) > 0.0)
            {
                prediction /= fsa[action];
            }

            // Update the best actions
            if (std::abs(m_maxPA - prediction) < DBL_EPSILON) // m_maxPA == prediction
            {
                m_maxPAActions.push_back(action);
            }
            else if (m_maxPA < prediction)
            {
                m_maxPAActions.clear();
                m_maxPAActions.push_back(action);
                m_maxPA = prediction;
            }
        }
    }

    double PredictionArray::max() const
    {
        if (m_maxPA == kInitialMaxPA)
        {
            throw std::runtime_error(
                "PredictionArray::m_maxPA has an invalid value.\n"
                "Didn't you set an empty MatchSet as an argument of the PredictionArray constructor?");
        }
        return m_maxPA;
    }

    double PredictionArray::predictionFor(int action) const
    {
        return m_pa.count(action) ? m_pa.at(action) : 0.0;
    }

    // SELECT ACTION
    int PredictionArray::selectAction(double epsilon, Random & random) const
    {
        if (epsilon > 0.0 && random.nextDouble() < epsilon)
        {
            if (m_paActions.empty())
            {
                throw std::runtime_error("PredictionArray::m_paActions is empty in PredictionArray::selectAction().");
            }

            // Choose random action
            return random.chooseFrom(m_paActions);
        }
        else
        {
            if (m_maxPAActions.empty())
            {
                throw std::runtime_error("PredictionArray::m_maxPAActions is empty in PredictionArray::selectAction().");
            }

            // Choose the best action
            return random.chooseFrom(m_maxPAActions);
        }
    }

}
