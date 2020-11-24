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

    void ExperimentHelper::runTrainIteration()
    {
        for (std::size_t i = 0; i < m_settings.explorationRepeat; ++i)
        {
            do
            {
                // Choose action
                const auto action = m_system->explore(m_trainEnvironment->situation());

                // Get reward
                const double reward = m_trainEnvironment->executeAction(action);
                m_system->reward(reward, m_trainEnvironment->isEndOfProblem());

                // Run callback if needed
                if (m_trainCallback != nullptr)
                {
                    m_trainCallback(*m_trainEnvironment);
                }
            } while (!m_trainEnvironment->isEndOfProblem());
        }
    }

    void ExperimentHelper::runTestIteration()
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
                    auto action = m_system->exploit(m_testEnvironment->situation(), m_settings.updateInExploitation);

                    // Get reward
                    double reward = m_testEnvironment->executeAction(action);

                    m_summaryRewardSum += reward / m_settings.exploitationRepeat;
                    m_summarySystemErrorSum += std::abs(reward - m_system->prediction()) / m_settings.exploitationRepeat;
                    m_summaryCoveringOccurrenceRateSum += static_cast<double>(m_system->isCoveringPerformed()) / m_settings.exploitationRepeat;

                    // Update for multistep problems
                    if (m_settings.updateInExploitation)
                    {
                        m_system->reward(reward, m_testEnvironment->isEndOfProblem());
                    }

                    rewardSum += reward;
                    systemErrorSum += std::abs(reward - m_system->prediction());
                    ++totalStepCount;

                    // Run callback if needed
                    if (m_testCallback != nullptr)
                    {
                        m_testCallback(*m_testEnvironment);
                    }
                } while (!m_testEnvironment->isEndOfProblem());

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
        , m_trainCallback(nullptr)
        , m_testCallback(nullptr)
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

    void ExperimentHelper::setTrainCallback(std::function<void(IEnvironment &)> callback)
    {
        m_trainCallback = callback;
    }

    void ExperimentHelper::setTestCallback(std::function<void(IEnvironment &)> callback)
    {
        m_testCallback = callback;
    }

    void ExperimentHelper::runIteration(std::size_t repeat)
    {
        if (!m_system)
        {
            throw std::domain_error("ExperimentHelper::constructExperiment() must be called before ExperimentHelper::runIteration().");
        }

        if (!m_trainEnvironment)
        {
            throw std::domain_error("ExperimentHelper::constructEnvironment() or ExperimentHelper::constructExplorationEnvironment() must be called before ExperimentHelper::runIteration().");
        }

        if (!m_testEnvironment)
        {
            throw std::domain_error("ExperimentHelper::constructEnvironment() or ExperimentHelper::constructExploitationEnvironment() must be called before ExperimentHelper::runIteration().");
        }

        for (std::size_t i = 0; i < repeat; ++i)
        {
            runTestIteration();
            runTrainIteration();
            ++m_iterationCount;
        }
    }

    void ExperimentHelper::switchToCondensationMode()
    {
        m_system->switchToCondensationMode();
    }

    IClassifierSystem & ExperimentHelper::system()
    {
        return *m_system;
    }

    const IClassifierSystem & ExperimentHelper::system() const
    {
        return *m_system;
    }

    IEnvironment & ExperimentHelper::trainEnv()
    {
        return *m_trainEnvironment;
    }

    const IEnvironment & ExperimentHelper::trainEnv() const
    {
        return *m_trainEnvironment;
    }

    IEnvironment & ExperimentHelper::testEnv()
    {
        return *m_testEnvironment;
    }

    const IEnvironment & ExperimentHelper::testEnv() const
    {
        return *m_testEnvironment;
    }

    std::size_t ExperimentHelper::iterationCount() const
    {
        return m_iterationCount;
    }

}
