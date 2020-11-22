#include "xcspp/helper/experiment_helper.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstdio> // std::printf, std::fflush
#include <cmath> // std::abs

namespace xcspp
{

    void ExperimentHelper::outputSummaryLogLine()
    {
        if (!m_alreadyOutputSummaryHeader)
        {
            if (m_settings.outputSummaryToStdout)
            {
                std::cout
                    << "  Iteration      Reward      SysErr     PopSize  CovOccRate   TotalStep\n"
                    << " ========== =========== =========== =========== =========== ===========" << std::endl;
            }
            if (m_summaryLogStream)
            {
                m_summaryLogStream << "Iteration,Reward,SysErr,PopSize,CovOccRate,TotalStep" << std::endl;
            }
            m_alreadyOutputSummaryHeader = true;
        }

        if (m_settings.outputSummaryToStdout)
        {
            std::printf("%11u %11.3f %11.3f %11.3f  %1.8f %11.3f\n",
                static_cast<unsigned int>(m_iterationCount + 1),
                m_summaryRewardSum / m_settings.summaryInterval,
                m_summarySystemErrorSum / m_settings.summaryInterval,
                m_summaryPopulationSizeSum / m_settings.summaryInterval,
                m_summaryCoveringOccurrenceRateSum / m_settings.summaryInterval,
                m_summaryStepCountSum / m_settings.summaryInterval);
            std::fflush(stdout);
        }

        if (m_summaryLogStream)
        {
            m_summaryLogStream
                << (m_iterationCount + 1) << ','
                << m_summaryRewardSum / m_settings.summaryInterval << ','
                << m_summarySystemErrorSum / m_settings.summaryInterval << ','
                << m_summaryPopulationSizeSum / m_settings.summaryInterval << ','
                << m_summaryCoveringOccurrenceRateSum / m_settings.summaryInterval << ','
                << m_summaryStepCountSum / m_settings.summaryInterval << std::endl;
        }

        m_summaryRewardSum = 0.0;
        m_summarySystemErrorSum = 0.0;
        m_summaryPopulationSizeSum = 0.0;
        m_summaryCoveringOccurrenceRateSum = 0.0;
        m_summaryStepCountSum = 0.0;
    }

    void ExperimentHelper::runExplorationIteration()
    {
        for (std::size_t i = 0; i < m_settings.explorationRepeat; ++i)
        {
            do
            {
                // Choose action
                const auto action = m_system->explore(m_explorationEnvironment->situation());

                // Get reward
                const double reward = m_explorationEnvironment->executeAction(action);
                m_system->reward(reward, m_explorationEnvironment->isEndOfProblem());

                // Run callback if needed
                if (m_explorationCallback != nullptr)
                {
                    m_explorationCallback(*m_explorationEnvironment);
                }
            } while (!m_explorationEnvironment->isEndOfProblem());
        }
    }

    void ExperimentHelper::runExploitationIteration()
    {
        if (m_settings.exploitationRepeat > 0)
        {
            std::size_t totalStepCount = 0;
            double rewardSum = 0.0;
            double systemErrorSum = 0.0;
            double populationSizeSum = 0.0;
            for (std::size_t i = 0; i < m_settings.exploitationRepeat; ++i)
            {
                do
                {
                    // Choose action
                    auto action = m_system->exploit(m_exploitationEnvironment->situation(), m_settings.updateInExploitation);

                    // Get reward
                    double reward = m_exploitationEnvironment->executeAction(action);

                    m_summaryRewardSum += reward / m_settings.exploitationRepeat;
                    m_summarySystemErrorSum += std::abs(reward - m_system->prediction()) / m_settings.exploitationRepeat;
                    m_summaryCoveringOccurrenceRateSum += static_cast<double>(m_system->isCoveringPerformed()) / m_settings.exploitationRepeat;

                    // Update for multistep problems
                    if (m_settings.updateInExploitation)
                    {
                        m_system->reward(reward, m_exploitationEnvironment->isEndOfProblem());
                    }

                    rewardSum += reward;
                    systemErrorSum += std::abs(reward - m_system->prediction());
                    ++totalStepCount;

                    // Run callback if needed
                    if (m_exploitationCallback != nullptr)
                    {
                        m_exploitationCallback(*m_exploitationEnvironment);
                    }
                } while (!m_exploitationEnvironment->isEndOfProblem());

                populationSizeSum += m_system->populationSize();
            }

            m_summaryPopulationSizeSum += static_cast<double>(m_system->populationSize());
            m_summaryStepCountSum += static_cast<double>(totalStepCount) / m_settings.exploitationRepeat;

            if (m_settings.summaryInterval > 0 && (m_iterationCount + 1) % m_settings.summaryInterval == 0)
            {
                outputSummaryLogLine();
            }

            m_rewardLogStream.writeLine(rewardSum / m_settings.exploitationRepeat);
            m_systemErrorLogStream.writeLine(systemErrorSum / m_settings.exploitationRepeat);
            m_populationSizeLogStream.writeLine(populationSizeSum / m_settings.exploitationRepeat);
            m_stepCountLogStream.writeLine(static_cast<double>(totalStepCount) / m_settings.exploitationRepeat);
        }
    }

    ExperimentHelper::ExperimentHelper(const ExperimentSettings & settings)
        : m_settings(settings)
        , m_explorationCallback(nullptr)
        , m_exploitationCallback(nullptr)
        , m_summaryLogStream(settings.outputSummaryFilename.empty() ? "" : (settings.outputFilenamePrefix + settings.outputSummaryFilename))
        , m_rewardLogStream(settings.outputRewardFilename.empty() ? "" : (settings.outputFilenamePrefix + settings.outputRewardFilename), settings.smaWidth, false)
        , m_systemErrorLogStream(settings.outputSystemErrorFilename.empty() ? "" : (settings.outputFilenamePrefix + settings.outputSystemErrorFilename), settings.smaWidth, false)
        , m_stepCountLogStream(settings.outputStepCountFilename.empty() ? "" : (settings.outputFilenamePrefix + settings.outputStepCountFilename), settings.smaWidth, false)
        , m_populationSizeLogStream(settings.outputPopulationSizeFilename.empty() ? "" : (settings.outputFilenamePrefix + settings.outputPopulationSizeFilename), false)
        , m_alreadyOutputSummaryHeader(false)
        , m_summaryRewardSum(0.0)
        , m_summarySystemErrorSum(0.0)
        , m_summaryPopulationSizeSum(0.0)
        , m_summaryCoveringOccurrenceRateSum(0.0)
        , m_summaryStepCountSum(0.0)
        , m_iterationCount(0)
    {
        if (!settings.inputClassifierFilename.empty())
        {
            m_system->loadPopulationCSVFile(settings.inputClassifierFilename, !settings.useInputClassifierToResume);
        }
    }

    void ExperimentHelper::setExplorationCallback(std::function<void(IEnvironment &)> callback)
    {
        m_explorationCallback = callback;
    }

    void ExperimentHelper::setExploitationCallback(std::function<void(IEnvironment &)> callback)
    {
        m_exploitationCallback = callback;
    }

    void ExperimentHelper::runIteration(std::size_t repeat)
    {
        if (!m_system)
        {
            throw std::domain_error("ExperimentHelper::constructExperiment() must be called before ExperimentHelper::runIteration().");
        }

        if (!m_explorationEnvironment)
        {
            throw std::domain_error("ExperimentHelper::constructEnvironment() or ExperimentHelper::constructExplorationEnvironment() must be called before ExperimentHelper::runIteration().");
        }

        if (!m_exploitationEnvironment)
        {
            throw std::domain_error("ExperimentHelper::constructEnvironment() or ExperimentHelper::constructExploitationEnvironment() must be called before ExperimentHelper::runIteration().");
        }

        for (std::size_t i = 0; i < repeat; ++i)
        {
            runExploitationIteration();
            runExplorationIteration();
            ++m_iterationCount;
        }
    }

    void ExperimentHelper::switchToCondensationMode()
    {
        m_system->switchToCondensationMode();
    }

    IClassifierSystem & ExperimentHelper::classifierSystem()
    {
        return *m_system;
    }

    const IClassifierSystem & ExperimentHelper::classifierSystem() const
    {
        return *m_system;
    }

    IEnvironment & ExperimentHelper::explorationEnvironment()
    {
        return *m_explorationEnvironment;
    }

    const IEnvironment & ExperimentHelper::explorationEnvironment() const
    {
        return *m_explorationEnvironment;
    }

    IEnvironment & ExperimentHelper::exploitationEnvironment()
    {
        return *m_exploitationEnvironment;
    }

    const IEnvironment & ExperimentHelper::exploitationEnvironment() const
    {
        return *m_exploitationEnvironment;
    }

    std::size_t ExperimentHelper::iterationCount() const
    {
        return m_iterationCount;
    }

}
