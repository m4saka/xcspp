#define __USE_MINGW_ANSI_STDIO 0
#include <iostream>
#include <string>
#include <cstdint> // std::uint64_t

#include <xcspp/xcspp.hpp>
#include <cxxopts.hpp>

#include "common/common.hpp"
#include "xcsr_option.hpp"

using namespace xcspp;

int main(int argc, char *argv[])
{
    // Prepare commandline options
    cxxopts::Options options(argv[0], "XCSR Classifier System");
    tool::xcsr::AddOptions(options);

    // Parse commandline options
    const auto parsedOptions = options.parse(argc, argv);

    // Show help if no environment is specified
    if (parsedOptions.count("help") || (!parsedOptions.count("rmux") /*&& !parsedOptions.count("csv")*/))
    {
        std::cout << options.help({"", "Experiment", "Environment", "XCSR parameter"}) << std::endl;
        return parsedOptions.count("help") ? 0 : 1;
    }

    // XCSR Hyperparameters
    const XCSRParams params = tool::xcsr::ParseXCSRParams(parsedOptions);
    tool::xcsr::OutputXCSRParams(params);

    // Initialize experiment helper
    const ExperimentSettings settings = tool::ParseExperimentSettings(parsedOptions);
    RealExperimentHelper experimentHelper(settings);

    if (parsedOptions.count("rmux"))
    {
        // Real multiplexer problem
        const auto & env = experimentHelper.constructTrainEnv<RealMultiplexerEnvironment>(parsedOptions["rmux"].as<int>(), parsedOptions["rmux-i"].as<unsigned int>());
        experimentHelper.constructTestEnv<RealMultiplexerEnvironment>(parsedOptions["rmux"].as<int>());

        experimentHelper.constructSystem<XCSR>(env.availableActions(), params);

        tool::RunExperiment(experimentHelper, parsedOptions["iter"].as<std::uint64_t>(), parsedOptions["condense-iter"].as<std::uint64_t>());
    }

    tool::OutputPopulation(experimentHelper, settings.outputFilenamePrefix + parsedOptions["coutput"].as<std::string>());

    return 0;
}
