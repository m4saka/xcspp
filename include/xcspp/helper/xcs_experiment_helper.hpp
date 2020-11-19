#pragma once
#include <memory> // std::unique_ptr
#include <functional> // std::function
#include <vector>
#include <unordered_set>
#include <cstddef> // std::size_t

#include "xcspp/xcs/xcs.hpp"
#include "xcspp/environment/ienvironment.hpp"
#include "experiment_settings.hpp"
#include "experiment_log_stream.hpp"

namespace xcspp
{

    class XCSExperimentHelper
    {
    private:
        const ExperimentSettings m_settings;
        std::unique_ptr<XCS> m_experiment;
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

        void runExploitationIteration();

        void runExplorationIteration();

    public:
        XCSExperimentHelper(const ExperimentSettings & settings, const XCSParams & params);

        ~XCSExperimentHelper() = default;

        template <class... Args>
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

        XCS & experiment();

        const XCS & experiment() const;

        IEnvironment & explorationEnvironment();

        const IEnvironment & explorationEnvironment() const;

        IEnvironment & exploitationEnvironment();

        const IEnvironment & exploitationEnvironment() const;
    };

    template <class... Args>
    void XCSExperimentHelper::constructExperiment(Args && ... args)
    {
        m_experiment = std::make_unique<XCS>(args...);
    }

    template <class Environment, class... Args>
    void XCSExperimentHelper::constructEnvironments(Args && ... args)
    {
        constructExplorationEnvironment(args...); // Note: std::forward is not used here because it is unsafe to move the same object twice
        constructExploitationEnvironment(std::forward<Args>(args)...);
    }

    template <class Environment, class... Args>
    void XCSExperimentHelper::constructExplorationEnvironment(Args && ... args)
    {
        m_explorationEnvironment = std::make_unique<Environment>(std::forward<Args>(args)...);
    }

    template <class Environment, class... Args>
    void XCSExperimentHelper::constructExploitationEnvironment(Args && ... args)
    {
        m_exploitationEnvironment = std::make_unique<Environment>(std::forward<Args>(args)...);
    }

}
