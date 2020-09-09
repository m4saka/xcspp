#pragma once
#include "classifier_ptr_set.hpp"
#include "random.hpp"

namespace xcspp
{

    class Population : public ClassifierPtrSet
    {
    public:
        // Constructor
        using ClassifierPtrSet::ClassifierPtrSet;

        // Destructor
        virtual ~Population() = default;

        // INSERT IN POPULATION
        void insertOrIncrementNumerosity(const ClassifierPtr & cl);

        // DELETE FROM POPULATION
        bool deleteExtraClassifiers(Random & random);
    };

}
