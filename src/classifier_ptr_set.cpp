#include "xcspp/classifier_ptr_set.hpp"
#include "xcspp/util/csv.hpp"

namespace xcspp
{

    namespace
    {
        std::unordered_set<ClassifierPtr> MakeSetFromClassifiers(const std::vector<Classifier> & classifiers, const XCSParams *pParams)
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
        : m_set(MakeSetFromClassifiers(initialClassifiers, pParams))
        , m_pParams(pParams)
        , m_availableActions(availableActions)
    {
    }

    void ClassifierPtrSet::setClassifiers(const std::vector<Classifier> & classifiers)
    {
        // Replace classifiers
        m_set.clear();
        m_set.reserve(classifiers.size());
        for (const auto & cl : classifiers)
        {
            m_set.emplace(std::make_shared<StoredClassifier>(cl, m_pParams));
        }
    }

    void ClassifierPtrSet::inputCSV(std::istream & is, bool initClassifierVariables)
    {
        auto classifiers = CSV::ReadClassifiers(is);
        if (initClassifierVariables)
        {
            for (auto & cl : classifiers)
            {
                cl.prediction = m_pParams->initialPrediction;
                cl.epsilon = m_pParams->initialEpsilon;
                cl.fitness = m_pParams->initialFitness;
                cl.experience = 0;
                cl.timeStamp = 0;
                cl.actionSetSize = 1;
                //cl.numerosity = 1; // commented out to keep macroclassifier as is
            }
        }
        setClassifiers(classifiers);
    }

    void ClassifierPtrSet::outputCSV(std::ostream & os) const
    {
        os << "Condition,Action,prediction,epsilon,F,exp,ts,as,n,acc\n";
        for (const auto & cl : m_set)
        {
            os  << cl->condition << ','
                << cl->action << ','
                << cl->prediction << ','
                << cl->epsilon << ','
                << cl->fitness << ','
                << cl->experience << ','
                << cl->timeStamp << ','
                << cl->actionSetSize << ','
                << cl->numerosity << ','
                << cl->accuracy() << '\n';
        }
    }

    bool ClassifierPtrSet::loadCSVFile(const std::string & filename, bool initClassifierVariables)
    {
        // Open file stream
        std::ifstream ifs(filename);
        if (!ifs.good())
        {
            return false;
        }

        // Read CSV
        inputCSV(ifs, initClassifierVariables);
        return true;
    }

    bool ClassifierPtrSet::saveCSVFile(const std::string & filename) const
    {
        // Open file stream
        std::ofstream ofs(filename);
        if (!ofs.good())
        {
            return false;
        }

        // Write CSV
        outputCSV(ofs);
        return true;
    }

}
