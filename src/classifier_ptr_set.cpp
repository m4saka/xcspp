#include "xcspp/classifier_ptr_set.hpp"

namespace xcspp
{

    namespace
    {
        std::unordered_set<ClassifierPtr> makeSetFromClassifiers(const std::vector<Classifier> & classifiers, const XCSParams *pParams)
        {
            std::unordered_set<ClassifierPtr> set;
            for (const auto & cl : classifiers)
            {
                set.emplace(std::make_shared<StoredClassifier>(cl, pParams));
            }
            return set;
        }
    }

    ClassifierPtrSet::ClassifierPtrSet(const XCSParams *pParams, const std::unordered_set<int> & availableActions)
        : m_pParams(pParams)
        , m_availableActions(availableActions)
    {
    }

    ClassifierPtrSet::ClassifierPtrSet(const std::unordered_set<ClassifierPtr> & set, const XCSParams *pParams, const std::unordered_set<int> & availableActions)
        : m_set(set)
        , m_pParams(pParams)
        , m_availableActions(availableActions)
    {
    }

    ClassifierPtrSet::ClassifierPtrSet(const std::vector<Classifier> & initialClassifiers, const XCSParams *pParams, const std::unordered_set<int> & availableActions)
        : m_set(makeSetFromClassifiers(initialClassifiers, pParams))
        , m_pParams(pParams)
        , m_availableActions(availableActions)
    {
    }

    void ClassifierPtrSet::dump(std::ostream & os) const
    {
        os << "Condition,int,prediction,epsilon,F,exp,ts,as,n,acc\n";
        for (const auto & cl : m_set)
        {
            os  << cl->condition << ","
                << cl->action << ","
                << cl->prediction << ","
                << cl->epsilon << ","
                << cl->fitness << ","
                << cl->experience << ","
                << cl->timeStamp << ","
                << cl->actionSetSize << ","
                << cl->numerosity << ","
                << cl->accuracy() << "\n";
        }
    }

}
