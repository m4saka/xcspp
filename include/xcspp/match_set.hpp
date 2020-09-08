#pragma once
#include <cstdint> // std::uint64_t

#include "classifier_ptr_set.hpp"
#include "population.hpp"
#include "xcs_params.hpp"

namespace xcspp
{

    class MatchSet : public ClassifierPtrSet
    {
    protected:
        bool m_isCoveringPerformed;

    public:
        // Constructor
        using ClassifierPtrSet::ClassifierPtrSet; // inherits all constructors from ClassifierPtrSet

        MatchSet(Population & population, const std::vector<int> & situation, std::uint64_t timeStamp, const XCSParams *pParams, const std::unordered_set<int> & availableActions);

        // Destructor
        virtual ~MatchSet() = default;

        // GENERATE MATCH SET
        void regenerate(Population & population, const std::vector<int> & situation, std::uint64_t timeStamp);

        // Get if covering is performed in the previous match set generation
        // (Call this function after constructor or regenerate())
        bool isCoveringPerformed() const;
    };

}
