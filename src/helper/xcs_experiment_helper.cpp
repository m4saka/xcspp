#include "xcspp/helper/xcs_experiment_helper.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstdio> // std::printf, std::fflush
#include <cmath> // std::abs

namespace xcspp
{

    std::vector<std::unique_ptr<XCS>> XCSExperimentHelper::MakeExperiments(
        const ExperimentSettings & settings,
        const std::unordered_set<int> & availableActions,
        const XCSParams & params)
    {
        std::vector<std::unique_ptr<XCS>> experiments;
        for (std::size_t i = 0; i < settings.seedSize; ++i)
        {
            experiments.push_back(
                std::make_unique<XCS>(availableActions, params)
            );
        }
        return experiments;
    }

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

    void XCSExperimentHelper::runExploitationIteration()
    {
        if (m_settings.exploitationRepeat > 0)
        {
            std::size_t totalStepCount = 0;
            double rewardSum = 0.0;
            double systemErrorSum = 0.0;
            double populationSizeSum = 0.0;
            for (std::size_t j = 0; j < m_settings.seedSize; ++j)
            {
                for (std::size_t k = 0; k < m_settings.exploitationRepeat; ++k)
                {
                    do
                    {
                        // Choose action
                        auto action = m_experiments[j]->exploit(m_exploitationEnvironments[j]->situation(), m_settings.updateInExploitation);

                        // Get reward
                        double reward = m_exploitationEnvironments[j]->executeAction(action);
                        m_summaryRewardSum += reward / m_settings.exploitationRepeat / m_settings.seedSize;
                        m_summarySystemErrorSum += std::abs(reward - m_experiments[j]->prediction()) / m_settings.exploitationRepeat / m_settings.seedSize;
                        m_summaryCoveringOccurrenceRateSum += static_cast<double>(m_experiments[j]->isCoveringPerformed()) / m_settings.exploitationRepeat / m_settings.seedSize;
                        if (m_settings.updateInExploitation)
                        {
                            m_experiments[j]->reward(reward, m_exploitationEnvironments[j]->isEndOfProblem());
                        }
                        rewardSum += reward;
                        systemErrorSum += std::abs(reward - m_experiments[j]->prediction());
                        ++totalStepCount;

                        // Run callback if needed
                        if (m_exploitationCallback != nullptr)
                        {
                            m_exploitationCallback(*m_exploitationEnvironments[j]);
                        }
                    } while (!m_exploitationEnvironments[j]->isEndOfProblem());

                    populationSizeSum += m_experiments[j]->populationSize();
                }
                m_summaryPopulationSizeSum += static_cast<double>(m_experiments[j]->populationSize()) / m_settings.seedSize;
            }

            m_summaryStepCountSum += static_cast<double>(totalStepCount) / m_settings.exploitationRepeat / m_settings.seedSize;

            if (m_settings.summaryInterval > 0 && (m_iterationCount + 1) % m_settings.summaryInterval == 0)
            {
                outputSummaryLogLine();
            }

            m_rewardLogStream.writeLine(rewardSum / m_settings.exploitationRepeat / m_settings.seedSize);
            m_systemErrorLogStream.writeLine(systemErrorSum / m_settings.exploitationRepeat / m_settings.seedSize);
            m_populationSizeLogStream.writeLine(populationSizeSum / m_settings.exploitationRepeat / m_settings.seedSize);
            m_stepCountLogStream.writeLine(static_cast<double>(totalStepCount) / m_settings.exploitationRepeat / m_settings.seedSize);
        }
    }

    void XCSExperimentHelper::runExplorationIteration()
    {
        for (std::size_t j = 0; j < m_settings.seedSize; ++j)
        {
            for (std::size_t k = 0; k < m_settings.explorationRepeat; ++k)
            {
                do
                {
                    // Choose action
                    auto action = m_experiments[j]->explore(m_explorationEnvironments[j]->situation());

                    // Get reward
                    double reward = m_explorationEnvironments[j]->executeAction(action);
                    m_experiments[j]->reward(reward, m_explorationEnvironments[j]->isEndOfProblem());

                    // Run callback if needed
                    if (m_explorationCallback != nullptr)
                    {
                        m_explorationCallback(*m_explorationEnvironments[j]);
                    }
                } while (!m_explorationEnvironments[j]->isEndOfProblem());
            }
        }
    }

    XCSExperimentHelper::XCSExperimentHelper(const ExperimentSettings & settings, const XCSParams & params)
        : m_settings(settings)
        , m_params(params)
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
            for (const auto & experiment : m_experiments)
            {
                experiment->loadPopulationCSVFile(settings.inputClassifierFilename, !settings.useInputClassifierToResume);
            }
        }
    }

    void XCSExperimentHelper::runIteration(std::size_t repeat)
    {
        if (m_experiments.empty())
        {
            throw std::domain_error("XCSExperimentHelper::constructEnvironments() must be called before XCSExperimentHelper::runIteration().");
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
        for (const auto & experiment : m_experiments)
        {
            experiment->switchToCondensationMode();
        }
    }

    std::size_t XCSExperimentHelper::seedSize() const
    {
        return m_settings.seedSize;
    }

    XCS & XCSExperimentHelper::experiment(std::size_t seedIdx)
    {
        return *m_experiments.at(seedIdx);
    }

    const XCS & XCSExperimentHelper::experiment(std::size_t seedIdx) const
    {
        return *m_experiments.at(seedIdx);
    }

    IEnvironment & XCSExperimentHelper::explorationEnvironment(std::size_t seedIdx)
    {
        return *m_explorationEnvironments.at(seedIdx);
    }

    const IEnvironment & XCSExperimentHelper::explorationEnvironment(std::size_t seedIdx) const
    {
        return *m_explorationEnvironments.at(seedIdx);
    }

    IEnvironment & XCSExperimentHelper::exploitationEnvironment(std::size_t seedIdx)
    {
        return *m_exploitationEnvironments.at(seedIdx);
    }

    const IEnvironment & XCSExperimentHelper::exploitationEnvironment(std::size_t seedIdx) const
    {
        return *m_exploitationEnvironments.at(seedIdx);
    }
}
