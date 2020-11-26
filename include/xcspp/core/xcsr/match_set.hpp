#pragma once
#include <cstdint> // std::uint64_t

#include "classifier_ptr_set.hpp"
#include "population.hpp"
#include "xcsr_params.hpp"
#include "xcspp/util/random.hpp"

namespace xcspp::xcsr
{

    class MatchSet : public ClassifierPtrSet
    {
    protected:
        bool m_isCoveringPerformed;

    public:
        // Constructor
        using ClassifierPtrSet::ClassifierPtrSet; // inherits all constructors from ClassifierPtrSet

        MatchSet(Population & population, const std::vector<int> & situation, std::uint64_t timeStamp, const XCSRParams *pParams, const std::unordered_set<int> & availableActions, Random & random);

        // Destructor
        virtual ~MatchSet() = default;

        // GENERATE MATCH SET
        void generateSet(Population & population, const std::vector<int> & situation, std::uint64_t timeStamp, Random & random);

        // Get if covering is performed in the previous match set generation
        // (Call this function after constructor or generateSet())
        bool isCoveringPerformed() const;
    };

}
