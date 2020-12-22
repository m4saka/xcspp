#include "xcspp/helper/experiment_iteration_logger.hpp"
#include <iostream>
#include <cmath> // std::abs

namespace xcspp
{
    
    ExperimentIterationLogger::ExperimentIterationLogger(const ExperimentSettings & settings)
        : m_rewardLogStream(settings.outputRewardFilename.empty() ? "" : (settings.outputFilenamePrefix + settings.outputRewardFilename), settings.smaWidth, false)
        , m_systemErrorLogStream(settings.outputSystemErrorFilename.empty() ? "" : (settings.outputFilenamePrefix + settings.outputSystemErrorFilename), settings.smaWidth, false)
        , m_populationSizeLogStream(settings.outputPopulationSizeFilename.empty() ? "" : (settings.outputFilenamePrefix + settings.outputPopulationSizeFilename), false)
        , m_stepCountLogStream(settings.outputStepCountFilename.empty() ? "" : (settings.outputFilenamePrefix + settings.outputStepCountFilename), settings.smaWidth, false)
        , m_exploitationRepeat(settings.exploitationRepeat)
        , m_currentRewardSum(0.0)
        , m_currentSystemErrorSum(0.0)
        , m_currentPopulationSizeSum(0.0)
        , m_currentStepCount(0)
    {
    }

    void ExperimentIterationLogger::oneStep(double reward, double prediction)
    {
        m_currentRewardSum += reward;
        m_currentSystemErrorSum += std::abs(reward - prediction);
        ++m_currentStepCount;
    }

    void ExperimentIterationLogger::oneExploitation(std::size_t populationSize)
    {
        m_currentPopulationSizeSum += static_cast<double>(populationSize);
    }

    void ExperimentIterationLogger::oneIteration()
    {
        m_rewardLogStream.writeLine(m_currentRewardSum / m_exploitationRepeat);
        m_systemErrorLogStream.writeLine(m_currentSystemErrorSum / m_exploitationRepeat);
        m_populationSizeLogStream.writeLine(m_currentPopulationSizeSum / m_exploitationRepeat);
        m_stepCountLogStream.writeLine(static_cast<double>(m_currentStepCount) / m_exploitationRepeat);

        m_currentRewardSum = 0.0;
        m_currentSystemErrorSum = 0.0;
        m_currentPopulationSizeSum = 0.0;
        m_currentStepCount = 0;
    }

}
