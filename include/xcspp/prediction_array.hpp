#pragma once
#include <vector>
#include <unordered_map>

#include "match_set.hpp"

namespace xcspp
{

    class PredictionArray
    {
    private:
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
        explicit PredictionArray(const MatchSet & matchSet);

        // Destructor
        ~PredictionArray() = default;

        double max() const;

        double predictionFor(int action) const;

        // SELECT ACTION
        // (You can use greedy selection by setting epsilon to zero.)
        int selectAction(double epsilon = 0.0) const;
    };

}
