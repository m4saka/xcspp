#pragma once
#include <fstream> // std::ofstream
#include <cstddef> // std::size_t
#include "experiment_settings.hpp"

namespace xcspp
{

    class ExperimentSummaryLogger
    {
    private:
        std::ofstream m_logStream;
        const bool m_outputsToStdout;
        const std::size_t m_intervalIteration;
        const std::size_t m_exploitationRepeat;

        double m_rewardSum;
        double m_systemErrorSum;
        double m_populationSizeSum;
        double m_coveringOccurrenceRateSum;
        double m_stepCountSum;

        bool m_alreadyOutputHeader;
        std::size_t m_currentIterationCount;
        std::size_t m_currentStepCount;

        void outputLogLine();

    public:
        explicit ExperimentSummaryLogger(const ExperimentSettings & settings);

        void oneStep(double reward, double prediction, bool coveringOccurred);

        void oneExploitation(std::size_t populationSize);

        void oneIteration();
    };

}
