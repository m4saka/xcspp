#include "xcspp/helper/xcs_experiment_helper.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstdio> // std::printf, std::fflush
#include <cmath> // std::abs

namespace xcspp
{

    void XCSExperimentHelper::outputSummaryLogLine()
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

    void XCSExperimentHelper::runExplorationIteration()
    {
        for (std::size_t i = 0; i < m_settings.explorationRepeat; ++i)
        {
            do
            {
                // Choose action
                auto action = m_experiment->explore(m_explorationEnvironment->situation());

                // Get reward
                double reward = m_explorationEnvironment->executeAction(action);
                m_experiment->reward(reward, m_explorationEnvironment->isEndOfProblem());

                // Run callback if needed
                if (m_explorationCallback != nullptr)
                {
                    m_explorationCallback(*m_explorationEnvironment);
                }
            } while (!m_explorationEnvironment->isEndOfProblem());
        }
    }

    void XCSExperimentHelper::runExploitationIteration()
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
                    auto action = m_experiment->exploit(m_exploitationEnvironment->situation(), m_settings.updateInExploitation);

                    // Get reward
                    double reward = m_exploitationEnvironment->executeAction(action);

                    m_summaryRewardSum += reward / m_settings.exploitationRepeat;
                    m_summarySystemErrorSum += std::abs(reward - m_experiment->prediction()) / m_settings.exploitationRepeat;
                    m_summaryCoveringOccurrenceRateSum += static_cast<double>(m_experiment->isCoveringPerformed()) / m_settings.exploitationRepeat;

                    // Update for multistep problems
                    if (m_settings.updateInExploitation)
                    {
                        m_experiment->reward(reward, m_exploitationEnvironment->isEndOfProblem());
                    }

                    rewardSum += reward;
                    systemErrorSum += std::abs(reward - m_experiment->prediction());
                    ++totalStepCount;

                    // Run callback if needed
                    if (m_exploitationCallback != nullptr)
                    {
                        m_exploitationCallback(*m_exploitationEnvironment);
                    }
                } while (!m_exploitationEnvironment->isEndOfProblem());

                populationSizeSum += m_experiment->populationSize();
            }

            m_summaryPopulationSizeSum += static_cast<double>(m_experiment->populationSize());
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

    XCSExperimentHelper::XCSExperimentHelper(const ExperimentSettings & settings, const XCSParams & params)
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
            m_experiment->loadPopulationCSVFile(settings.inputClassifierFilename, !settings.useInputClassifierToResume);
        }
    }

    void XCSExperimentHelper::setExplorationCallback(std::function<void(IEnvironment &)> callback)
    {
        m_explorationCallback = callback;
    }

    void XCSExperimentHelper::setExploitationCallback(std::function<void(IEnvironment &)> callback)
    {
        m_exploitationCallback = callback;
    }

    void XCSExperimentHelper::runIteration(std::size_t repeat)
    {
        if (!m_experiment)
        {
            throw std::domain_error("XCSExperimentHelper::constructExperiment() must be called before XCSExperimentHelper::runIteration().");
        }

        if (!m_explorationEnvironment)
        {
            throw std::domain_error("XCSExperimentHelper::constructEnvironment() or XCSExperimentHelper::constructExplorationEnvironment() must be called before XCSExperimentHelper::runIteration().");
        }

        if (!m_exploitationEnvironment)
        {
            throw std::domain_error("XCSExperimentHelper::constructEnvironment() or XCSExperimentHelper::constructExploitationEnvironment() must be called before XCSExperimentHelper::runIteration().");
        }

        for (std::size_t i = 0; i < repeat; ++i)
        {
            runExploitationIteration();
            runExplorationIteration();
            ++m_iterationCount;
        }
    }

    void XCSExperimentHelper::switchToCondensationMode()
    {
        m_experiment->switchToCondensationMode();
    }

    XCS & XCSExperimentHelper::experiment()
    {
        return *m_experiment;
    }

    const XCS & XCSExperimentHelper::experiment() const
    {
        return *m_experiment;
    }

    IEnvironment & XCSExperimentHelper::explorationEnvironment()
    {
        return *m_explorationEnvironment;
    }

    const IEnvironment & XCSExperimentHelper::explorationEnvironment() const
    {
        return *m_explorationEnvironment;
    }

    IEnvironment & XCSExperimentHelper::exploitationEnvironment()
    {
        return *m_exploitationEnvironment;
    }

    const IEnvironment & XCSExperimentHelper::exploitationEnvironment() const
    {
        return *m_exploitationEnvironment;
    }
}
