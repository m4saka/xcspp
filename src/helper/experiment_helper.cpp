#include "xcspp/helper/experiment_helper.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstdio> // std::printf, std::fflush
#include <cmath> // std::abs

namespace xcspp
{

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
                    m_trainCallback();
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
                    const auto action = m_system->exploit(m_testEnvironment->situation(), m_settings.updateInExploitation);

                    // Get reward
                    const double reward = m_testEnvironment->executeAction(action);

                    // Update for multistep problems
                    if (m_settings.updateInExploitation)
                    {
                        m_system->reward(reward, m_testEnvironment->isEndOfProblem());
                    }

                    m_summaryLogger.oneStep(reward, m_system->prediction(), m_system->isCoveringPerformed());

                    rewardSum += reward;
                    systemErrorSum += std::abs(reward - m_system->prediction());
                    ++totalStepCount;

                    // Run callback if needed
                    if (m_testCallback != nullptr)
                    {
                        m_testCallback();
                    }
                } while (!m_testEnvironment->isEndOfProblem());

                populationSizeSum += m_system->populationSize();
            }

            m_summaryLogger.oneIteration(m_system->populationSize());

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
        , m_rewardLogStream(settings.outputRewardFilename.empty() ? "" : (settings.outputFilenamePrefix + settings.outputRewardFilename), settings.smaWidth, false)
        , m_systemErrorLogStream(settings.outputSystemErrorFilename.empty() ? "" : (settings.outputFilenamePrefix + settings.outputSystemErrorFilename), settings.smaWidth, false)
        , m_stepCountLogStream(settings.outputStepCountFilename.empty() ? "" : (settings.outputFilenamePrefix + settings.outputStepCountFilename), settings.smaWidth, false)
        , m_populationSizeLogStream(settings.outputPopulationSizeFilename.empty() ? "" : (settings.outputFilenamePrefix + settings.outputPopulationSizeFilename), false)
        , m_summaryLogger(settings)
        , m_iterationCount(0)
    {
        if (!settings.inputClassifierFilename.empty())
        {
            m_system->loadPopulationCSVFile(settings.inputClassifierFilename, settings.initializeInputClassifier);
        }
    }

    void ExperimentHelper::setTrainCallback(std::function<void()> callback)
    {
        m_trainCallback = callback;
    }

    void ExperimentHelper::setTestCallback(std::function<void()> callback)
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
