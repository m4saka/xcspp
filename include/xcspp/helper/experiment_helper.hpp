#pragma once
#include <memory> // std::unique_ptr
#include <functional> // std::function
#include <vector>
#include <unordered_set>
#include <stdexcept>
#include <cstddef> // std::size_t

#include "xcspp/core/xcs/xcs.hpp"
#include "xcspp/environment/ienvironment.hpp"
#include "experiment_settings.hpp"
#include "experiment_log_stream.hpp"
#include "experiment_iteration_logger.hpp"
#include "experiment_summary_logger.hpp"

namespace xcspp
{

    template <typename T>
    class BasicExperimentHelper
    {
    private:
        const ExperimentSettings m_settings;
        std::unique_ptr<IBasicClassifierSystem<T>> m_system;
        std::unique_ptr<IBasicEnvironment<T>> m_trainEnvironment;
        std::unique_ptr<IBasicEnvironment<T>> m_testEnvironment;
        std::function<void()> m_trainCallback;
        std::function<void()> m_testCallback;

        std::size_t m_iterationCount;

        // Logger for every iteration
        // (reward, system error, population size, step count)
        ExperimentIterationLogger m_iterationLogger;

        // Logger for summary log
        ExperimentSummaryLogger m_summaryLogger;

        void runTrainIteration();

        void runTestIteration();

    public:
        explicit BasicExperimentHelper(const ExperimentSettings & settings);

        ~BasicExperimentHelper() = default;

        template <class ClassifierSystem, class... Args>
        ClassifierSystem & constructSystem(Args && ... args);

        template <class Environment, class... Args>
        Environment & constructTrainEnv(Args && ... args);

        template <class Environment, class... Args>
        Environment & constructTestEnv(Args && ... args);

        void setTrainCallback(std::function<void()> callback);

        void setTestCallback(std::function<void()> callback);

        void runIteration(std::size_t repeat = 1);

        void switchToCondensationMode();

        IBasicClassifierSystem<T> & system();

        const IBasicClassifierSystem<T> & system() const;

        IBasicEnvironment<T> & trainEnv();

        const IBasicEnvironment<T> & trainEnv() const;

        IBasicEnvironment<T> & testEnv();

        const IBasicEnvironment<T> & testEnv() const;

        std::size_t iterationCount() const;
    };

    template <typename T>
    void BasicExperimentHelper<T>::runTrainIteration()
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

    template <typename T>
    void BasicExperimentHelper<T>::runTestIteration()
    {
        if (m_settings.exploitationRepeat > 0)
        {
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

                    m_iterationLogger.oneStep(reward, m_system->prediction());
                    m_summaryLogger.oneStep(reward, m_system->prediction(), m_system->isCoveringPerformed());

                    // Run callback if needed
                    if (m_testCallback != nullptr)
                    {
                        m_testCallback();
                    }
                } while (!m_testEnvironment->isEndOfProblem());

                m_iterationLogger.oneExploitation(m_system->populationSize());
                m_summaryLogger.oneExploitation(m_system->populationSize());
            }

            m_iterationLogger.oneIteration();
            m_summaryLogger.oneIteration();
        }
    }

    template <typename T>
    BasicExperimentHelper<T>::BasicExperimentHelper(const ExperimentSettings & settings)
        : m_settings(settings)
        , m_trainCallback(nullptr)
        , m_testCallback(nullptr)
        , m_iterationLogger(settings)
        , m_summaryLogger(settings)
        , m_iterationCount(0)
    {
        if (!settings.inputClassifierFilename.empty())
        {
            m_system->loadPopulationCSVFile(settings.inputClassifierFilename, settings.initializeInputClassifier);
        }
    }

    template <typename T>
    template <class ClassifierSystem, class... Args>
    ClassifierSystem & BasicExperimentHelper<T>::constructSystem(Args && ... args)
    {
        m_system = std::make_unique<ClassifierSystem>(args...);
        if (!m_system)
        {
            throw std::bad_alloc();
        }
        return *dynamic_cast<ClassifierSystem *>(m_system.get());
    }

    template <typename T>
    template <class Environment, class... Args>
    Environment & BasicExperimentHelper<T>::constructTrainEnv(Args && ... args)
    {
        m_trainEnvironment = std::make_unique<Environment>(std::forward<Args>(args)...);
        if (!m_trainEnvironment)
        {
            throw std::bad_alloc();
        }
        return *dynamic_cast<Environment *>(m_trainEnvironment.get());
    }

    template <typename T>
    template <class Environment, class... Args>
    Environment & BasicExperimentHelper<T>::constructTestEnv(Args && ... args)
    {
        m_testEnvironment = std::make_unique<Environment>(std::forward<Args>(args)...);
        if (!m_testEnvironment)
        {
            throw std::bad_alloc();
        }
        return *dynamic_cast<Environment *>(m_testEnvironment.get());
    }

    template <typename T>
    void BasicExperimentHelper<T>::setTrainCallback(std::function<void()> callback)
    {
        m_trainCallback = callback;
    }

    template <typename T>
    void BasicExperimentHelper<T>::setTestCallback(std::function<void()> callback)
    {
        m_testCallback = callback;
    }

    template <typename T>
    void BasicExperimentHelper<T>::runIteration(std::size_t repeat)
    {
        if (!m_system)
        {
            throw std::domain_error("ExperimentHelper: constructExperiment() must be called before runIteration().");
        }

        if (!m_trainEnvironment)
        {
            throw std::domain_error("ExperimentHelper: constructEnvironment() or constructExplorationEnvironment() must be called before runIteration().");
        }

        if (!m_testEnvironment)
        {
            throw std::domain_error("ExperimentHelper: constructEnvironment() or constructExploitationEnvironment() must be called before runIteration().");
        }

        for (std::size_t i = 0; i < repeat; ++i)
        {
            runTestIteration();
            runTrainIteration();
            ++m_iterationCount;
        }
    }

    template <typename T>
    void BasicExperimentHelper<T>::switchToCondensationMode()
    {
        m_system->switchToCondensationMode();
    }

    template <typename T>
    IBasicClassifierSystem<T> & BasicExperimentHelper<T>::system()
    {
        return *m_system;
    }

    template <typename T>
    const IBasicClassifierSystem<T> & BasicExperimentHelper<T>::system() const
    {
        return *m_system;
    }

    template <typename T>
    IBasicEnvironment<T> & BasicExperimentHelper<T>::trainEnv()
    {
        return *m_trainEnvironment;
    }

    template <typename T>
    const IBasicEnvironment<T> & BasicExperimentHelper<T>::trainEnv() const
    {
        return *m_trainEnvironment;
    }

    template <typename T>
    IBasicEnvironment<T> & BasicExperimentHelper<T>::testEnv()
    {
        return *m_testEnvironment;
    }

    template <typename T>
    const IBasicEnvironment<T> & BasicExperimentHelper<T>::testEnv() const
    {
        return *m_testEnvironment;
    }

    template <typename T>
    std::size_t BasicExperimentHelper<T>::iterationCount() const
    {
        return m_iterationCount;
    }

    using ExperimentHelper = BasicExperimentHelper<int>;
    using RealExperimentHelper = BasicExperimentHelper<double>;

}
