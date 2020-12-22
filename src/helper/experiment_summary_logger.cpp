#include "xcspp/helper/experiment_summary_logger.hpp"
#include <iostream>
#include <cmath> // std::abs

namespace xcspp
{

    void ExperimentSummaryLogger::outputLogLine()
    {
        if (!m_alreadyOutputHeader)
        {
            if (m_outputsToStdout)
            {
                std::cout
                    << "  Iteration      Reward      SysErr     PopSize  CovOccRate   TotalStep\n"
                    << " ========== =========== =========== =========== =========== ===========" << std::endl;
            }
            if (m_logStream)
            {
                m_logStream << "Iteration,Reward,SysErr,PopSize,CovOccRate,TotalStep" << std::endl;
            }
            m_alreadyOutputHeader = true;
        }

        if (m_outputsToStdout)
        {
            std::printf("%11u %11.3f %11.3f %11.3f  %1.8f %11.3f\n",
                static_cast<unsigned int>(m_currentIterationCount + 1),
                m_rewardSum / m_intervalIteration,
                m_systemErrorSum / m_intervalIteration,
                m_populationSizeSum / m_intervalIteration,
                m_coveringOccurrenceRateSum / m_intervalIteration,
                m_stepCountSum / m_intervalIteration);
            std::fflush(stdout);
        }

        if (m_logStream)
        {
            m_logStream
                << (m_currentIterationCount + 1) << ','
                << m_rewardSum / m_intervalIteration << ','
                << m_systemErrorSum / m_intervalIteration << ','
                << m_populationSizeSum / m_intervalIteration << ','
                << m_coveringOccurrenceRateSum / m_intervalIteration << ','
                << m_stepCountSum / m_intervalIteration << std::endl;
        }

        m_rewardSum = 0.0;
        m_systemErrorSum = 0.0;
        m_populationSizeSum = 0.0;
        m_coveringOccurrenceRateSum = 0.0;
        m_stepCountSum = 0.0;
    }

    ExperimentSummaryLogger::ExperimentSummaryLogger(const ExperimentSettings & settings)
        : m_logStream(settings.outputSummaryFilename.empty() ? "" : (settings.outputFilenamePrefix + settings.outputSummaryFilename))
        , m_outputsToStdout(settings.outputSummaryToStdout)
        , m_intervalIteration(settings.summaryInterval)
        , m_exploitationRepeat(settings.exploitationRepeat)
        , m_rewardSum(0.0)
        , m_systemErrorSum(0.0)
        , m_populationSizeSum(0.0)
        , m_coveringOccurrenceRateSum(0.0)
        , m_stepCountSum(0.0)
        , m_alreadyOutputHeader(false)
        , m_currentIterationCount(0)
        , m_currentStepCount(0)
    {
    }

    void ExperimentSummaryLogger::oneStep(double reward, double prediction, bool coveringOccurred)
    {
        m_rewardSum += reward / m_exploitationRepeat;
        m_systemErrorSum += std::abs(reward - prediction) / m_exploitationRepeat;
        m_coveringOccurrenceRateSum += static_cast<double>(coveringOccurred) / m_exploitationRepeat;
        ++m_currentStepCount;
    }

    void ExperimentSummaryLogger::oneExploitation(std::size_t populationSize)
    {
        m_populationSizeSum += static_cast<double>(populationSize) / m_exploitationRepeat;
        m_stepCountSum += static_cast<double>(m_currentStepCount) / m_exploitationRepeat;
        m_currentStepCount = 0;
    }

    void ExperimentSummaryLogger::oneIteration()
    {
        // Periodic log output
        if (m_intervalIteration > 0 && (m_currentIterationCount + 1) % m_intervalIteration == 0)
        {
            outputLogLine();
        }

        ++m_currentIterationCount;
    }

}
