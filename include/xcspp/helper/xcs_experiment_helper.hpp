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
        std::vector<std::unique_ptr<XCS>> m_experiments;
        std::vector<std::unique_ptr<IEnvironment>> m_explorationEnvironments;
        std::vector<std::unique_ptr<IEnvironment>> m_exploitationEnvironments;
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
        void constructExperiments(Args && ... args);

        template <class Environment, class... Args>
        void constructEnvironments(Args && ... args);

        void setExplorationCallback(std::function<void(IEnvironment &)> callback);

        void setExploitationCallback(std::function<void(IEnvironment &)> callback);

        void runIteration(std::size_t repeat = 1);

        void switchToCondensationMode();

        std::size_t seedSize() const;

        XCS & experiment(std::size_t seedIdx = 0);

        const XCS & experiment(std::size_t seedIdx = 0) const;

        IEnvironment & explorationEnvironment(std::size_t seedIdx = 0);

        const IEnvironment & explorationEnvironment(std::size_t seedIdx = 0) const;

        IEnvironment & exploitationEnvironment(std::size_t seedIdx = 0);

        const IEnvironment & exploitationEnvironment(std::size_t seedIdx = 0) const;
    };

    template <class... Args>
    void XCSExperimentHelper::constructExperiments(Args && ... args)
    {
        // Clear experiment array
        m_experiments.clear();

        // Construct experiments
        m_experiments.reserve(m_settings.seedSize);
        for (std::size_t i = 0; i < m_settings.seedSize; ++i)
        {
            // Note: std::forward is not used here because it is unsafe when seedSize > 1
            m_experiments.push_back(
                std::make_unique<XCS>(args...)
            );
        }

        // Shrink array
        m_experiments.shrink_to_fit();
    }

    template <class Environment, class... Args>
    void XCSExperimentHelper::constructEnvironments(Args && ... args)
    {
        // Clear environment arrays
        m_explorationEnvironments.clear();
        m_exploitationEnvironments.clear();

        // Construct environments
        m_explorationEnvironments.reserve(m_settings.seedSize);
        m_exploitationEnvironments.reserve(m_settings.seedSize);
        for (std::size_t i = 0; i < m_settings.seedSize; ++i)
        {
            // Note: std::forward is not used here because it is unsafe due to multiple calls
            m_explorationEnvironments.push_back(std::make_unique<Environment>(args...));
            m_exploitationEnvironments.push_back(std::make_unique<Environment>(args...));
        }

        // Shrink arrays
        m_explorationEnvironments.shrink_to_fit();
        m_exploitationEnvironments.shrink_to_fit();
    }

}
