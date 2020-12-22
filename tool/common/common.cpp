#include "common.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib> // std::exit

namespace xcspp::tool
{

    void AddExperimentOptions(cxxopts::Options & options)
    {
        options.add_options("Experiment")
            ("summary-interval", "The iteration interval of summary log output", cxxopts::value<uint64_t>()->default_value("5000"), "COUNT")
            ("p,prefix", "The filename prefix for log file output", cxxopts::value<std::string>()->default_value(""), "PREFIX")
            ("S,soutput", "The filename of summary log csv output", cxxopts::value<std::string>()->default_value("summary.csv"), "FILENAME")
            ("o,coutput", "The filename of classifier csv output", cxxopts::value<std::string>()->default_value("classifier.csv"), "FILENAME")
            ("r,routput", "The filename of reward log csv output", cxxopts::value<std::string>()->default_value(""), "FILENAME")
            ("E,seoutput", "The filename of system error log csv output", cxxopts::value<std::string>()->default_value(""), "FILENAME")
            ("n,noutput", "The filename of macro-classifier count log csv output", cxxopts::value<std::string>()->default_value(""), "FILENAME")
            ("nsoutput", "The filename of number-of-step log csv output in the multi-step problem", cxxopts::value<std::string>()->default_value(""), "FILENAME")
            ("cinput", "The classifier csv filename for initial population", cxxopts::value<std::string>()->default_value(""), "FILENAME")
            ("cinput-init", "Whether to initialize p/epsilon/F/exp/ts/as to defaults", cxxopts::value<bool>()->default_value("false"), "true/false")
            ("i,iter", "The number of iterations", cxxopts::value<uint64_t>()->default_value("100000"), "COUNT")
            ("condense-iter", "The number of iterations for the Wilson's rule condensation method (chi=0, mu=0) after normal iterations", cxxopts::value<uint64_t>()->default_value("0"), "COUNT")
            //("avg-seeds", "The number of different random seeds for averaging the reward and the macro-classifier count", cxxopts::value<uint64_t>()->default_value("1"), "COUNT")
            ("explore", "The number of exploration performed in each train iteration", cxxopts::value<uint64_t>()->default_value("1"), "COUNT")
            ("exploit", "The number of exploitation (= test mode) performed in each test iteration (set \"0\" if you don't need evaluation)", cxxopts::value<uint64_t>()->default_value("1"), "COUNT")
            ("exploit-upd", "Whether to update classifier parameters in test mode (\"auto\": false for single-step & true for multi-step)", cxxopts::value<std::string>()->default_value("auto"), "auto/true/false")
            ("sma", "The width of the simple moving average for the reward log", cxxopts::value<uint64_t>()->default_value("1"), "COUNT");
    }

    ExperimentSettings ParseExperimentSettings(const cxxopts::ParseResult & parsedOptions)
    {
        ExperimentSettings settings;
        settings.explorationRepeat = parsedOptions["explore"].as<uint64_t>();
        settings.exploitationRepeat = parsedOptions["exploit"].as<uint64_t>();

        // Determine whether to update classifier parameters in exploitation
        if (parsedOptions["exploit-upd"].as<std::string>() == "true")
        {
            settings.updateInExploitation = true;
        }
        else if (parsedOptions["exploit-upd"].as<std::string>() == "false")
        {
            settings.updateInExploitation = false;
        }
        else if (parsedOptions["exploit-upd"].as<std::string>() == "auto")
        {
            settings.updateInExploitation = parsedOptions.count("blc");
        }
        else
        {
            std::cerr << "Error: Unknown value for --exploit-upd (" << parsedOptions["exploit-upd"].as<std::string>() << ")" << std::endl;
            std::exit(1);
        }

        settings.summaryInterval = parsedOptions["summary-interval"].as<uint64_t>();
        settings.outputFilenamePrefix = parsedOptions["prefix"].as<std::string>();
        settings.outputSummaryToStdout = true;
        settings.outputSummaryFilename = parsedOptions["soutput"].as<std::string>();
        settings.outputRewardFilename = parsedOptions["routput"].as<std::string>();
        settings.outputSystemErrorFilename = parsedOptions["seoutput"].as<std::string>();
        settings.outputPopulationSizeFilename = parsedOptions["noutput"].as<std::string>();
        settings.outputStepCountFilename = parsedOptions["nsoutput"].as<std::string>();
        settings.inputClassifierFilename = parsedOptions["cinput"].as<std::string>();
        settings.initializeInputClassifier = parsedOptions["cinput-init"].as<bool>();
        settings.smaWidth = parsedOptions["sma"].as<uint64_t>();

        return settings;
    }

    void OutputPopulation(const IExperimentHelper & experimentHelper, const std::string & filename)
    {
        std::ofstream ofs;
        std::ostream & os = filename.empty() ? std::cout : ofs; // if filename is empty, output to stdout
        if (!filename.empty())
        {
            ofs.open(filename);
        }
        if (os)
        {
            experimentHelper.outputPopulationCSV(os);
        }
    }

    void RunExperiment(IExperimentHelper & experimentHelper, std::uint64_t iterationCount, std::uint64_t condensationIterationCount)
    {
        experimentHelper.runIteration(iterationCount);
        if (condensationIterationCount > 0)
        {
            experimentHelper.switchToCondensationMode();
            experimentHelper.runIteration(condensationIterationCount);
        }
    }

}
