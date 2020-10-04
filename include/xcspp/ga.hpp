#pragma once
#include <memory> // std::shared_ptr, std::make_shared
#include <vector>
#include <unordered_set>
#include <cstdint> // std::uint64_t
#include <cstddef> // std::size_t

#include "random.hpp"
#include "classifier_ptr_set.hpp"
#include "population.hpp"

namespace xcspp
{

    namespace GA
    {
        // RUN GA (refer to ActionSet::runGA() for the former part)
        void Run(
            ClassifierPtrSet & actionSet,
            const std::vector<int> & situation,
            Population & population,
            const std::unordered_set<int> & availableActions,
            const XCSParams *pParams,
            Random & random);
    };

}
