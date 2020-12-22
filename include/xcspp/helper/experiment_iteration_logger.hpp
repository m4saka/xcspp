#pragma once
#include "experiment_log_stream.hpp"
#include "experiment_settings.hpp"

namespace xcspp
{

    class ExperimentIterationLogger
    {
    private:
        SMAExperimentLogStream m_rewardLogStream;
        SMAExperimentLogStream m_systemErrorLogStream;
        ExperimentLogStream m_populationSizeLogStream;
        SMAExperimentLogStream m_stepCountLogStream;
        const std::size_t m_exploitationRepeat;

        double m_currentRewardSum;
        double m_currentSystemErrorSum;
        double m_currentPopulationSizeSum;
        std::size_t m_currentStepCount;

    public:
        explicit ExperimentIterationLogger(const ExperimentSettings & settings);

        void oneStep(double reward, double prediction);

        void oneExploitation(std::size_t populationSize);

        void oneIteration();
    };

}
