#pragma once
#include <memory> // std::unique_ptr
#include <functional> // std::function
#include <vector>
#include <unordered_set>
#include <cstddef> // std::size_t

#include "xcspp/core/xcs/xcs.hpp"
#include "xcspp/environment/ienvironment.hpp"
#include "experiment_settings.hpp"
#include "experiment_log_stream.hpp"

namespace xcspp
{

    class ExperimentHelper
    {
    private:
        const ExperimentSettings m_settings;
        std::unique_ptr<IClassifierSystem> m_system;
        std::unique_ptr<IEnvironment> m_explorationEnvironment;
        std::unique_ptr<IEnvironment> m_exploitationEnvironment;
        std::function<void(IEnvironment &)> m_explorationCallback;
        std::function<void(IEnvironment &)> m_exploitationCallback;
        std::ofstream m_summaryLogStream;
        bool m_outputSummaryLogFile;
        SMAExperimentLogStream m_rewardLogStream;
        SMAExperimentLogStream m_systemErrorLogStream;
        SMAExperimentLogStream m_stepCountLogStream;
        ExperimentLogStream m_populationSizeLogStream;
        bool m_alreadyOutputSummaryHeader;
        double m_summaryRewardSum;
        double m_summarySystemErrorSum;
        double m_summaryPopulationSizeSum;
        double m_summaryCoveringOccurrenceRateSum;
        double m_summaryStepCountSum;
        std::size_t m_iterationCount;

        void outputSummaryLogLine();

        void runExplorationIteration();

        void runExploitationIteration();

    public:
        explicit ExperimentHelper(const ExperimentSettings & settings);

        ~ExperimentHelper() = default;

        template <class ClassifierSystem, class... Args>
        ClassifierSystem & constructClassifierSystem(Args && ... args);

        template <class Environment, class... Args>
        Environment & constructExplorationEnvironment(Args && ... args);

        template <class Environment, class... Args>
        Environment & constructExploitationEnvironment(Args && ... args);

        void setExplorationCallback(std::function<void(IEnvironment &)> callback);

        void setExploitationCallback(std::function<void(IEnvironment &)> callback);

        void runIteration(std::size_t repeat = 1);

        void switchToCondensationMode();

        IClassifierSystem & classifierSystem();

        const IClassifierSystem & classifierSystem() const;

        IEnvironment & explorationEnvironment();

        const IEnvironment & explorationEnvironment() const;

        IEnvironment & exploitationEnvironment();

        const IEnvironment & exploitationEnvironment() const;

        std::size_t iterationCount() const;
    };

    template <class ClassifierSystem, class... Args>
    ClassifierSystem & ExperimentHelper::constructClassifierSystem(Args && ... args)
    {
        m_system = std::make_unique<ClassifierSystem>(args...);
        if (!m_system)
        {
            throw std::bad_alloc();
        }
        return *dynamic_cast<ClassifierSystem *>(m_system.get());
    }

    template <class Environment, class... Args>
    Environment & ExperimentHelper::constructExplorationEnvironment(Args && ... args)
    {
        m_explorationEnvironment = std::make_unique<Environment>(std::forward<Args>(args)...);
        if (!m_explorationEnvironment)
        {
            throw std::bad_alloc();
        }
        return *dynamic_cast<Environment *>(m_explorationEnvironment.get());
    }

    template <class Environment, class... Args>
    Environment & ExperimentHelper::constructExploitationEnvironment(Args && ... args)
    {
        m_exploitationEnvironment = std::make_unique<Environment>(std::forward<Args>(args)...);
        if (!m_exploitationEnvironment)
        {
            throw std::bad_alloc();
        }
        return *dynamic_cast<Environment *>(m_exploitationEnvironment.get());
    }

}
