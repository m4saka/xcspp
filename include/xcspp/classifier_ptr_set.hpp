#pragma once
#include <vector>
#include <unordered_set>
#include <memory> // std::shared_ptr

#include "classifier.hpp"
#include "xcs_params.hpp"
#include "random.hpp"

namespace xcspp
{

    using ClassifierPtr = std::shared_ptr<StoredClassifier>;

    class ClassifierPtrSet
    {
    protected:
        std::unordered_set<ClassifierPtr> m_set;
        const XCSParams * const m_pParams;
        const std::unordered_set<int> m_availableActions;

    public:
        // Constructor
        ClassifierPtrSet(const XCSParams *pParams, const std::unordered_set<int> & availableActions);

        ClassifierPtrSet(const std::unordered_set<ClassifierPtr> & set, const XCSParams *pParams, const std::unordered_set<int> & availableActions);

        ClassifierPtrSet(const std::vector<Classifier> & initialClassifiers, const XCSParams *pParams, const std::unordered_set<int> & availableActions);

        // Destructor
        virtual ~ClassifierPtrSet() = default;

        void dump(std::ostream & os) const;

        // --- The functions below are just the wrapper for std::unordered_set<ClassifierPtr> ---

        auto empty() const noexcept
        {
            return m_set.empty();
        }

        auto size() const noexcept
        {
            return m_set.size();
        }

        auto begin() const noexcept
        {
            return m_set.begin();
        }

        auto end() const noexcept
        {
            return m_set.end();
        }

        auto cbegin() const noexcept
        {
            return m_set.cbegin();
        }

        auto cend() const noexcept
        {
            return m_set.cend();
        }

        template <class... Args>
        auto insert(Args && ... args)
        {
            return m_set.insert(std::forward<Args>(args)...);
        }

        template <class... Args>
        auto emplace(Args && ... args)
        {
            return m_set.emplace(std::forward<Args>(args)...);
        }

        template <class... Args>
        auto erase(Args && ... args)
        {
            return m_set.erase(std::forward<Args>(args)...);
        }

        void clear() noexcept
        {
            m_set.clear();
        }

        template <class... Args>
        void swap(Args && ... args)
        {
            m_set.swap(std::forward<Args>(args)...);
        }

        template <class... Args>
        auto find(Args && ... args) const
        {
            return m_set.find(std::forward<Args>(args)...);
        }

        template <class... Args>
        auto count(Args && ... args) const
        {
            return m_set.count(std::forward<Args>(args)...);
        }
    };

}
