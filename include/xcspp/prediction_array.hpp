#pragma once
#include <vector>
#include <unordered_map>

#include "match_set.hpp"
#include "xcs_params.hpp"

namespace xcspp
{

    class PredictionArray
    {
    private:
        const XCSParams * const m_pParams;

        // PA (Prediction Array)
        std::unordered_map<int, double> m_pa;

        // Array of PA keys (for random action selection)
        std::vector<int> m_paActions;

        // The maximum value of PA
        double m_maxPA;

        // The best actions of PA
        std::vector<int> m_maxPAActions;

    public:
        // GENERATE PREDICTION ARRAY
        PredictionArray(const MatchSet & matchSet, const XCSParams *pParams);

        // Destructor
        ~PredictionArray() = default;

        double max() const;

        double predictionFor(int action) const;

        // SELECT ACTION
        // (You can use greedy selection by setting epsilon to zero.)
        int selectAction(double epsilon, Random & random) const;
    };

}
