#include "common.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib> // std::exit

namespace xcspp
{

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

    void OutputPopulation(const ExperimentHelper & experimentHelper, const std::string & filename)
    {
        std::ofstream ofs;
        std::ostream & os = filename.empty() ? std::cout : ofs; // if filename is empty, output to stdout
        if (!filename.empty())
        {
            ofs.open(filename);
        }
        if (os)
        {
            experimentHelper.system().outputPopulationCSV(os);
        }
    }

    void RunExperiment(ExperimentHelper & experimentHelper, std::uint64_t iterationCount, std::uint64_t condensationIterationCount)
    {
        experimentHelper.runIteration(iterationCount);
        if (condensationIterationCount > 0)
        {
            experimentHelper.switchToCondensationMode();
            experimentHelper.runIteration(condensationIterationCount);
        }
    }

}
