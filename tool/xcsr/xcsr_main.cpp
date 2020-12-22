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
    cxxopts::Options options(argv[0], "XCS Classifier System");
    tool::xcs::AddOptions(options);

    // Parse commandline options
    const auto parsedOptions = options.parse(argc, argv);

    // Show help if no environment is specified
    if (parsedOptions.count("help") || (!parsedOptions.count("mux") && !parsedOptions.count("parity") && !parsedOptions.count("majority") && !parsedOptions.count("blc") && !parsedOptions.count("csv")))
    {
        std::cout << options.help({"", "Experiment", "Environment", "XCS parameter"}) << std::endl;
        return parsedOptions.count("help") ? 0 : 1;
    }

    // XCS Hyperparameters
    const XCSParams params = tool::xcs::ParseXCSParams(parsedOptions);
    tool::xcs::OutputXCSParams(params);

    // Initialize experiment helper
    const ExperimentSettings settings = tool::ParseExperimentSettings(parsedOptions);
    ExperimentHelper experimentHelper(settings);

    if (parsedOptions.count("mux"))
    {
        // Multiplexer problem
        const auto & env = experimentHelper.constructTrainEnv<MultiplexerEnvironment>(parsedOptions["mux"].as<int>(), parsedOptions["mux-i"].as<unsigned int>());
        experimentHelper.constructTestEnv<MultiplexerEnvironment>(parsedOptions["mux"].as<int>());

        experimentHelper.constructSystem<XCS>(env.availableActions(), params);

        tool::RunExperiment(experimentHelper, parsedOptions["iter"].as<std::uint64_t>(), parsedOptions["condense-iter"].as<std::uint64_t>());
    }
    else if (parsedOptions.count("parity"))
    {
        // Even-parity problem
        const auto & env = experimentHelper.constructTrainEnv<EvenParityEnvironment>(parsedOptions["parity"].as<int>());
        experimentHelper.constructTestEnv<EvenParityEnvironment>(parsedOptions["parity"].as<int>());

        experimentHelper.constructSystem<XCS>(env.availableActions(), params);

        tool::RunExperiment(experimentHelper, parsedOptions["iter"].as<std::uint64_t>(), parsedOptions["condense-iter"].as<std::uint64_t>());
    }
    else if (parsedOptions.count("majority"))
    {
        // Majority-on problem
        const auto & env = experimentHelper.constructTrainEnv<MajorityOnEnvironment>(parsedOptions["majority"].as<int>());
        experimentHelper.constructTestEnv<MajorityOnEnvironment>(parsedOptions["majority"].as<int>());

        experimentHelper.constructSystem<XCS>(env.availableActions(), params);

        tool::RunExperiment(experimentHelper, parsedOptions["iter"].as<std::uint64_t>(), parsedOptions["condense-iter"].as<std::uint64_t>());
    }
    else if (parsedOptions.count("blc"))
    {
        // Block world problem
        const auto & trainEnv = experimentHelper.constructTrainEnv<BlockWorldEnvironment>(parsedOptions["blc"].as<std::string>(), parsedOptions["max-step"].as<uint64_t>(), parsedOptions["blc-3bit"].as<bool>(), parsedOptions["blc-diag"].as<bool>());
        const auto & testEnv = experimentHelper.constructTestEnv<BlockWorldEnvironment>(parsedOptions["blc"].as<std::string>(), parsedOptions["max-step"].as<uint64_t>(), parsedOptions["blc-3bit"].as<bool>(), parsedOptions["blc-diag"].as<bool>());

        auto & xcs = experimentHelper.constructSystem<XCS>(trainEnv.availableActions(), params);

        // Prepare trace output
        std::ofstream traceLogStream;
        const bool outputTraceLog = !parsedOptions["blc-output-trace"].as<std::string>().empty();
        if (outputTraceLog)
        {
            traceLogStream.open(parsedOptions["blc-output-trace"].as<std::string>());
        }
        experimentHelper.setTrainCallback([outputTraceLog, &traceLogStream, &env = trainEnv]() {
            if (outputTraceLog)
            {
                if (env.lastStep() <= 1)
                {
                    traceLogStream << "(" << env.lastInitialX() << "," << env.lastInitialY() << ")";
                }
                traceLogStream << "(" << env.lastX() << "," << env.lastY() << ")";
                if (env.isEndOfProblem())
                {
                    traceLogStream << " Explore" << std::endl;
                }
            }
        });
        experimentHelper.setTestCallback([outputTraceLog, &traceLogStream, &env = testEnv]() {
            if (outputTraceLog)
            {
                if (env.lastStep() <= 1)
                {
                    traceLogStream << "(" << env.lastInitialX() << "," << env.lastInitialY() << ")";
                }
                traceLogStream << "(" << env.lastX() << "," << env.lastY() << ")";
                if (env.isEndOfProblem())
                {
                    traceLogStream << " Exploit" << std::endl;
                }
            }
        });

        tool::RunExperiment(experimentHelper, parsedOptions["iter"].as<std::uint64_t>(), parsedOptions["condense-iter"].as<std::uint64_t>());

        // Output best action map
        if (!parsedOptions["blc-output-best"].as<std::string>().empty())
        {
            std::ofstream ofs(parsedOptions["blc-output-best"].as<std::string>());

            const bool useUnicode = parsedOptions["blc-output-best-uni"].as<bool>();

            for (int y = 0; y < testEnv.worldHeight(); ++y)
            {
                for (int x = 0; x < testEnv.worldWidth(); ++x)
                {
                    if (testEnv.isEmpty(x, y))
                    {
                        // Output the selected action
                        const auto situation = testEnv.situation(x, y);
                        const int action = xcs.exploit(situation);
                        if (useUnicode)
                        {
                            switch (action)
                            {
                            case 0:
                                ofs << u8"↑";
                                break;

                            case 1:
                                ofs << u8"↗";
                                break;

                            case 2:
                                ofs << u8"→";
                                break;

                            case 3:
                                ofs << u8"↘";
                                break;

                            case 4:
                                ofs << u8"↓";
                                break;

                            case 5:
                                ofs << u8"↙";
                                break;

                            case 6:
                                ofs << u8"←";
                                break;

                            case 7:
                                ofs << u8"↖";
                                break;

                            default:
                                ofs << u8"？";
                            }
                        }
                        else
                        {
                            ofs << action;
                        }
                    }
                    else
                    {
                        // Obstacle or Food
                        const char c = testEnv.getBlockChar(x, y);
                        if (useUnicode)
                        {
                            switch (c)
                            {
                            case 'Q':
                                ofs << u8"◆";
                                break;

                            case 'F':
                                ofs << u8"Ｆ";
                                break;

                            case 'G':
                                ofs << u8"Ｇ";
                                break;

                            default:
                                ofs << u8"■";
                            }
                        }
                        else
                        {
                            ofs << c;
                        }
                    }
                }
                ofs << std::endl;
            }
        }
    }
    else if (parsedOptions.count("csv"))
    {
        // CSV file
        const std::string trainFilename = parsedOptions["csv"].as<std::string>();
        const std::string testFilename = parsedOptions.count("csv-test") ? parsedOptions["csv-test"].as<std::string>() : trainFilename;

        const auto & env = experimentHelper.constructTrainEnv<DatasetEnvironment>(CSV::ReadDatasetFromFile(trainFilename), parsedOptions["csv-random"].as<bool>());
        experimentHelper.constructTestEnv<DatasetEnvironment>(CSV::ReadDatasetFromFile(testFilename), parsedOptions["csv-random"].as<bool>());

        experimentHelper.constructSystem<XCS>(env.availableActions(), params);

        tool::RunExperiment(experimentHelper, parsedOptions["iter"].as<std::uint64_t>(), parsedOptions["condense-iter"].as<std::uint64_t>());
    }

    tool::OutputPopulation(experimentHelper, settings.outputFilenamePrefix + parsedOptions["coutput"].as<std::string>());

    return 0;
}
