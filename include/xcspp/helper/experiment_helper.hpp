#pragma once
#include <memory> // std::unique_ptr
#include <functional> // std::function
#include <vector>
#include <unordered_set>
#include <cstddef> // std::size_t

#include "xcspp/xcs.hpp"
#include "xcspp/environment/ienvironment.hpp"
#include "experiment_settings.hpp"
#include "experiment_log_stream.hpp"

namespace xcspp
{

    class ExperimentHelper
    {
    private:
        const ExperimentSettings m_settings;
        const XCSParams m_params;
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

        static std::vector<std::unique_ptr<XCS>> makeExperiments(
            const ExperimentSettings & settings,
            const std::unordered_set<int> & availableActions,
            const XCSParams & params);

    public:
        ExperimentHelper(const ExperimentSettings & settings, const XCSParams & params);

        ~ExperimentHelper() = default;

        template <class Environment, class... Args>
        void constructEnvironments(Args && ... args);

        void runIteration(std::size_t repeat = 1);

        void switchToCondensationMode();

        std::size_t seedCount() const;

        XCS & experimentAt(std::size_t seedIdx);

        const XCS & experimentAt(std::size_t seedIdx) const;

        IEnvironment & explorationEnvironmentAt(std::size_t seedIdx);

        const IEnvironment & explorationEnvironmentAt(std::size_t seedIdx) const;

        IEnvironment & exploitationEnvironmentAt(std::size_t seedIdx);

        const IEnvironment & exploitationEnvironmentAt(std::size_t seedIdx) const;

        void dumpPopulation(std::size_t seedIdx, std::ostream & os) const;
    };

    template <class Environment, class... Args>
    void ExperimentHelper::constructEnvironments(Args && ... args)
    {
        // Clear environment arrays
        m_explorationEnvironments.clear();
        m_exploitationEnvironments.clear();

        if (m_settings.seedCount == 0)
        {
            return;
        }

        // Construct environments
        for (std::size_t i = 0; i < m_settings.seedCount; ++i)
        {
            m_explorationEnvironments.push_back(std::make_unique<Environment>(std::forward<Args>(args)...));
            m_exploitationEnvironments.push_back(std::make_unique<Environment>(std::forward<Args>(args)...));
        }

        // Construct experiments
        m_experiments = makeExperiments(m_settings, m_explorationEnvironments.at(0)->availableActions(), m_params);
    }

}
