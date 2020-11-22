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
        std::unique_ptr<IClassifierSystem> m_experiment;
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

        template <class Experiment, class... Args>
        void constructExperiment(Args && ... args);

        template <class Environment, class... Args>
        void constructEnvironments(Args && ... args);

        template <class Environment, class... Args>
        void constructExplorationEnvironment(Args && ... args);

        template <class Environment, class... Args>
        void constructExploitationEnvironment(Args && ... args);

        void setExplorationCallback(std::function<void(IEnvironment &)> callback);

        void setExploitationCallback(std::function<void(IEnvironment &)> callback);

        void runIteration(std::size_t repeat = 1);

        void switchToCondensationMode();

        IClassifierSystem & experiment();

        const IClassifierSystem & experiment() const;

        IEnvironment & explorationEnvironment();

        const IEnvironment & explorationEnvironment() const;

        IEnvironment & exploitationEnvironment();

        const IEnvironment & exploitationEnvironment() const;

        std::size_t iterationCount() const;
    };

    template <class Experiment, class... Args>
    void ExperimentHelper::constructExperiment(Args && ... args)
    {
        m_experiment = std::make_unique<Experiment>(args...);
    }

    template <class Environment, class... Args>
    void ExperimentHelper::constructEnvironments(Args && ... args)
    {
        constructExplorationEnvironment<Environment>(args...); // Note: std::forward is not used here because it is unsafe to move the same object twice
        constructExploitationEnvironment<Environment>(std::forward<Args>(args)...);
    }

    template <class Environment, class... Args>
    void ExperimentHelper::constructExplorationEnvironment(Args && ... args)
    {
        m_explorationEnvironment = std::make_unique<Environment>(std::forward<Args>(args)...);
    }

    template <class Environment, class... Args>
    void ExperimentHelper::constructExploitationEnvironment(Args && ... args)
    {
        m_exploitationEnvironment = std::make_unique<Environment>(std::forward<Args>(args)...);
    }

}
