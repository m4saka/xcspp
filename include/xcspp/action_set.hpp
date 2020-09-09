#pragma once
#include <vector>
#include <unordered_set>
#include <cstdint>

#include "classifier_ptr_set.hpp"
#include "population.hpp"
#include "match_set.hpp"
#include "ga.hpp"
#include "xcs_params.hpp"
#include "random.hpp"

namespace xcspp
{

    class ActionSet : public ClassifierPtrSet
    {
    private:
        // UPDATE FITNESS
        void updateFitness();

        // DO ACTION SET SUBSUMPTION
        void doSubsumption(Population & population);

    public:
        // Constructor
        ActionSet(const XCSParams *pParams, const std::unordered_set<int> & availableActions);

        ActionSet(const MatchSet & matchSet, int action, const XCSParams *pParams, const std::unordered_set<int> & availableActions);

        // Destructor
        virtual ~ActionSet() = default;

        // GENERATE ACTION SET
        void generateSet(const MatchSet & matchSet, int action);

        void copyTo(ActionSet & dest);

        // RUN GA (refer to GA::run() for the latter part)
        void runGA(const std::vector<int> & situation, Population & population, std::uint64_t timeStamp, Random & random);

        // UPDATE SET
        void update(double p, Population & population);
    };

}
