#pragma once
#include <memory> // std::unique_ptr
#include <functional> // std::function
#include <vector>
#include <unordered_set>
#include <cstddef> // std::size_t

#include "xcspp/core/xcs/xcs.hpp"
#include "xcspp/environment/ienvironment.hpp"
#include "experiment_settings.hpp"
#include "experiment_log_stream.hpp"

namespace xcspp
{

    class ExperimentHelper
    {
    private:
        const ExperimentSettings m_settings;
        std::unique_ptr<IClassifierSystem> m_system;
        std::unique_ptr<IEnvironment> m_trainEnvironment;
        std::unique_ptr<IEnvironment> m_testEnvironment;
        std::function<void()> m_trainCallback;
        std::function<void()> m_testCallback;
        std::ofstream m_summaryLogStream;
        bool m_outputSummaryLogFile;
        SMAExperimentLogStream m_rewardLogStream;
        SMAExperimentLogStream m_systemErrorLogStream;
        SMAExperimentLogStream m_stepCountLogStream;
        ExperimentLogStream m_populationSizeLogStream;
        bool m_alreadyOutputSummaryHeader;
        double m_summaryRewardSum;
        double m_summarySystemErrorSum;
        double m_summaryPopulationSizeSum;
        double m_summaryCoveringOccurrenceRateSum;
        double m_summaryStepCountSum;
        std::size_t m_iterationCount;

        void outputSummaryLogLine();

        void runTrainIteration();

        void runTestIteration();

    public:
        explicit ExperimentHelper(const ExperimentSettings & settings);

        ~ExperimentHelper() = default;

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

        IClassifierSystem & system();

        const IClassifierSystem & system() const;

        IEnvironment & trainEnv();

        const IEnvironment & trainEnv() const;

        IEnvironment & testEnv();

        const IEnvironment & testEnv() const;

        std::size_t iterationCount() const;
    };

    template <class ClassifierSystem, class... Args>
    ClassifierSystem & ExperimentHelper::constructSystem(Args && ... args)
    {
        m_system = std::make_unique<ClassifierSystem>(args...);
        if (!m_system)
        {
            throw std::bad_alloc();
        }
        return *dynamic_cast<ClassifierSystem *>(m_system.get());
    }

    template <class Environment, class... Args>
    Environment & ExperimentHelper::constructTrainEnv(Args && ... args)
    {
        m_trainEnvironment = std::make_unique<Environment>(std::forward<Args>(args)...);
        if (!m_trainEnvironment)
        {
            throw std::bad_alloc();
        }
        return *dynamic_cast<Environment *>(m_trainEnvironment.get());
    }

    template <class Environment, class... Args>
    Environment & ExperimentHelper::constructTestEnv(Args && ... args)
    {
        m_testEnvironment = std::make_unique<Environment>(std::forward<Args>(args)...);
        if (!m_testEnvironment)
        {
            throw std::bad_alloc();
        }
        return *dynamic_cast<Environment *>(m_testEnvironment.get());
    }

}
