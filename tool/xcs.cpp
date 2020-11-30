#define __USE_MINGW_ANSI_STDIO 0
#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <unordered_set>
#include <cstdint>
#include <cstddef>

#include <xcspp/xcspp.hpp>
#include <cxxopts.hpp>

#include "common.hpp"

using namespace xcspp;

void AddOptions(cxxopts::Options & options)
{
    const XCSParams defaultParams;
    options
        .add_options()
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
        ("sma", "The width of the simple moving average for the reward log", cxxopts::value<uint64_t>()->default_value("1"), "COUNT")
        ("m,mux", "Use the multiplexer problem", cxxopts::value<int>(), "LENGTH")
        ("mux-i", "Class imbalance level i of the multiplexer problem (used only in train iterations)", cxxopts::value<unsigned int>()->default_value("0"), "LEVEL")
        ("parity", "Use the even-parity problem", cxxopts::value<int>(), "LENGTH")
        ("majority", "Use the majority-on problem", cxxopts::value<int>(), "LENGTH")
        ("blc", "Use the block world problem", cxxopts::value<std::string>(), "FILENAME")
        ("blc-3bit", "Use 3-bit representation for each block in a situation", cxxopts::value<bool>()->default_value("false"), "true/false")
        ("blc-diag", "Allow diagonal actions in the block world problem", cxxopts::value<bool>()->default_value("true"), "true/false")
        ("blc-output-best", "Output the parsedOptions of the desired action for blocks in the block world problem", cxxopts::value<std::string>()->default_value(""), "FILENAME")
        ("blc-output-best-uni", "Use UTF-8 square & arrow characters for --blc-output", cxxopts::value<bool>()->default_value("false"), "true/false")
        ("blc-output-trace", "Output the coordinate of the animat in the block world problem", cxxopts::value<std::string>()->default_value(""), "FILENAME")
        ("max-step", "The maximum number of steps in the multi-step problem", cxxopts::value<uint64_t>()->default_value("50"))
        ("c,csv", "The csv file to train", cxxopts::value<std::string>(), "FILENAME")
        ("csv-test", "The csv file to test", cxxopts::value<std::string>(), "FILENAME")
        ("csv-random", "Whether to choose lines in random order from the csv file", cxxopts::value<bool>()->default_value("true"), "true/false")
        ("csv-estimate", "The csv file to estimate the outputs", cxxopts::value<std::string>(), "FILENAME")
        ("csv-output-best", "Output the parsedOptions of the desired action for the situations in the csv file specified by --csv-estimate", cxxopts::value<std::string>(), "FILENAME")
        ("N,max-population", "The maximum size of the population", cxxopts::value<uint64_t>()->default_value(std::to_string(defaultParams.n)), "COUNT")
        ("alpha", "The fall of rate in the fitness evaluation", cxxopts::value<double>()->default_value(std::to_string(defaultParams.alpha)), "ALPHA")
        ("beta", "The learning rate for updating fitness, prediction, prediction error, and action set size estimate in XCS's classifiers", cxxopts::value<double>()->default_value(std::to_string(defaultParams.beta)), "BETA")
        ("epsilon-0", "The error threshold under which the accuracy of a classifier is set to one", cxxopts::value<double>()->default_value(std::to_string(defaultParams.alpha)), "EPSILON_0")
        ("nu", "The exponent in the power function for the fitness evaluation", cxxopts::value<double>()->default_value(std::to_string(defaultParams.nu)), "NU")
        ("gamma", "The discount rate in multi-step problems", cxxopts::value<double>()->default_value(std::to_string(defaultParams.gamma)), "GAMMA")
        ("theta-ga", "The threshold for the GA application in an action set", cxxopts::value<uint64_t>()->default_value(std::to_string(defaultParams.thetaGA)), "THETA_GA")
        ("chi", "The probability of applying crossover", cxxopts::value<double>()->default_value(std::to_string(defaultParams.chi)), "CHI")
        ("x-method", "The method of crossover operator", cxxopts::value<std::string>()->default_value("uniform"), "uniform/one-point/two-point")
        ("mu", "The probability of mutating one allele and the action", cxxopts::value<double>()->default_value(std::to_string(defaultParams.mu)), "MU")
        ("theta-del", "The experience threshold over which the fitness of a classifier may be considered in its deletion probability", cxxopts::value<double>()->default_value(std::to_string(defaultParams.thetaDel)), "THETA_DEL")
        ("delta", "The fraction of the mean fitness of the population below which the fitness of a classifier may be considered in its vote for deletion", cxxopts::value<double>()->default_value(std::to_string(defaultParams.delta)), "DELTA")
        ("theta-sub", "The experience of a classifier required to be a subsumer", cxxopts::value<double>()->default_value(std::to_string(defaultParams.thetaSub)), "THETA_SUB")
        ("tau", "The tournament size for selection [Butz et al., 2003] (set \"0\" to use the roulette-wheel selection)", cxxopts::value<double>()->default_value(std::to_string(defaultParams.tau)), "TAU")
        ("s,p-sharp", "The probability of using a don't care symbol in an allele when covering", cxxopts::value<double>()->default_value(std::to_string(defaultParams.dontCareProbability)), "P_SHARP")
        ("initial-prediction", "The initial prediction value when generating a new classifier", cxxopts::value<double>()->default_value(std::to_string(defaultParams.initialPrediction)), "P_I")
        ("initial-prediction-error", "The initial prediction error value when generating a new classifier", cxxopts::value<double>()->default_value(std::to_string(defaultParams.initialEpsilon)), "EPSILON_I")
        ("initial-fitness", "The initial fitness value when generating a new classifier", cxxopts::value<double>()->default_value(std::to_string(defaultParams.initialFitness)), "F_I")
        ("p-explr", "The probability during action selection of choosing the action uniform randomly", cxxopts::value<double>()->default_value(std::to_string(defaultParams.exploreProbability)), "P_EXPLR")
        ("theta-mna", "The minimal number of actions that must be present in a match set [M], or else covering will occur. Use \"0\" to set automatically.", cxxopts::value<uint64_t>()->default_value(std::to_string(defaultParams.thetaMna)), "THETA_MNA")
        ("do-ga-subsumption", "Whether offspring are to be tested for possible logical subsumption by parents", cxxopts::value<bool>()->default_value(defaultParams.doGASubsumption ? "true" : "false"), "true/false")
        ("do-action-set-subsumption", "Whether action sets are to be tested for subsuming classifiers", cxxopts::value<bool>()->default_value(defaultParams.doActionSetSubsumption ? "true" : "false"), "true/false")
        ("do-action-mutation", "Whether to apply mutation to the action", cxxopts::value<bool>()->default_value(defaultParams.doActionMutation ? "true" : "false"), "true/false")
        ("mam", "Whether to use the moyenne adaptive modifee (MAM) for updating the prediction and the prediction error of classifiers", cxxopts::value<bool>()->default_value(defaultParams.useMAM ? "true" : "false"), "true/false")
        ("h,help", "Show this help");
}

XCSParams ParseXCSParams(const cxxopts::ParseResult & parsedOptions)
{
    XCSParams params;
    if (parsedOptions.count("max-population"))
        params.n = parsedOptions["max-population"].as<uint64_t>();
    if (parsedOptions.count("alpha"))
        params.alpha = parsedOptions["alpha"].as<double>();
    if (parsedOptions.count("beta"))
        params.beta = parsedOptions["beta"].as<double>();
    if (parsedOptions.count("epsilon-0"))
        params.epsilonZero = parsedOptions["epsilon-0"].as<double>();
    if (parsedOptions.count("nu"))
        params.nu = parsedOptions["nu"].as<double>();
    if (parsedOptions.count("gamma"))
        params.gamma = parsedOptions["gamma"].as<double>();
    if (parsedOptions.count("theta-ga"))
        params.thetaGA = parsedOptions["theta-ga"].as<uint64_t>();
    if (parsedOptions.count("chi"))
        params.chi = parsedOptions["chi"].as<double>();
    if (parsedOptions.count("mu"))
        params.mu = parsedOptions["mu"].as<double>();
    if (parsedOptions.count("theta-del"))
        params.thetaDel = parsedOptions["theta-del"].as<double>();
    if (parsedOptions.count("delta"))
        params.delta = parsedOptions["delta"].as<double>();
    if (parsedOptions.count("theta-sub"))
        params.thetaSub = parsedOptions["theta-sub"].as<double>();
    if (parsedOptions.count("tau"))
        params.tau = parsedOptions["tau"].as<double>();
    if (parsedOptions.count("p-sharp"))
        params.dontCareProbability = parsedOptions["p-sharp"].as<double>();
    if (parsedOptions.count("initial-prediction"))
        params.initialPrediction = parsedOptions["initial-prediction"].as<double>();
    if (parsedOptions.count("initial-prediction-error"))
        params.initialEpsilon = parsedOptions["initial-prediction-error"].as<double>();
    if (parsedOptions.count("initial-fitness"))
        params.initialFitness = parsedOptions["initial-fitness"].as<double>();
    if (parsedOptions.count("p-explr"))
        params.exploreProbability = parsedOptions["p-explr"].as<double>();
    if (parsedOptions.count("theta-mna"))
        params.thetaMna = parsedOptions["theta-mna"].as<uint64_t>();
    if (parsedOptions.count("do-ga-subsumption"))
        params.doGASubsumption = parsedOptions["do-ga-subsumption"].as<bool>();
    if (parsedOptions.count("do-action-set-subsumption"))
        params.doActionSetSubsumption = parsedOptions["do-action-set-subsumption"].as<bool>();
    if (parsedOptions.count("do-action-mutation"))
        params.doActionMutation = parsedOptions["do-action-mutation"].as<bool>();
    if (parsedOptions.count("mam"))
        params.useMAM = parsedOptions["mam"].as<bool>();

    // Determine crossover method
    if (parsedOptions["x-method"].as<std::string>() == "uniform")
    {
        params.crossoverMethod = XCSParams::CrossoverMethod::kUniformCrossover;
    }
    else if (parsedOptions["x-method"].as<std::string>() == "one-point")
    {
        params.crossoverMethod = XCSParams::CrossoverMethod::kOnePointCrossover;
    }
    else if (parsedOptions["x-method"].as<std::string>() == "two-point")
    {
        params.crossoverMethod = XCSParams::CrossoverMethod::kTwoPointCrossover;
    }
    else
    {
        std::cerr << "Error: Unknown value for --x-method (" << parsedOptions["x-method"].as<std::string>() << ")" << std::endl;
        std::exit(1);
    }

    return params;
}

void OutputXCSParams(const XCSParams & params)
{
    // Output parameters
    std::cout << "[ XCS General Parameters ]\n";
    std::cout << "               N = " << params.n << '\n';
    std::cout << "            beta = " << params.beta << '\n';
    std::cout << "           alpha = " << params.alpha << '\n';
    std::cout << "       epsilon_0 = " << params.epsilonZero << '\n';
    std::cout << "              nu = " << params.nu << '\n';
    std::cout << "           gamma = " << params.gamma << '\n';
    std::cout << "        theta_GA = " << params.thetaGA << '\n';
    std::cout << "             chi = " << params.chi << '\n';
    std::cout << "              mu = " << params.mu << '\n';
    std::cout << "       theta_del = " << params.thetaDel << '\n';
    std::cout << "           delta = " << params.delta << '\n';
    std::cout << "       theta_sub = " << params.thetaSub << '\n';
    std::cout << "             P_# = " << params.dontCareProbability << '\n';
    std::cout << "             p_I = " << params.initialPrediction << '\n';
    std::cout << "       epsilon_I = " << params.initialEpsilon << '\n';
    std::cout << "             F_I = " << params.initialFitness << '\n';
    std::cout << "         p_explr = " << params.exploreProbability << '\n';
    if (params.thetaMna != 0) std::cout << "       theta_mna = " << params.thetaMna << '\n';
    std::cout << " doGASubsumption = " << (params.doGASubsumption ? "true" : "false") << '\n';
    std::cout << " doASSubsumption = " << (params.doActionSetSubsumption ? "true" : "false") << '\n';
    std::cout << " crossoverMethod = ";
    switch (params.crossoverMethod)
    {
    case XCSParams::CrossoverMethod::kUniformCrossover:
        std::cout << "uniform\n";
        break;
    case XCSParams::CrossoverMethod::kOnePointCrossover:
        std::cout << "one-point\n";
        break;
    case XCSParams::CrossoverMethod::kTwoPointCrossover:
        std::cout << "two-point\n";
        break;
    }
    std::cout << '\n';

    // Output optional settings
    std::stringstream ss;
    if (params.tau > 0.0 && params.tau <= 1.0)
        ss << "     GASelection = tournament (tau=" << params.tau << ")\n";
    else
        ss << "     GASelection = roulette\n";
    if (!params.doActionMutation)
        ss << "doActionMutation = false\n";
    if (!params.useMAM)
        ss << "          useMAM = false\n";
    std::string str = ss.str();
    if (!str.empty())
    {
        std::cout << "[ XCS Optional Settings ]\n";
        std::cout << str << '\n';
    }

    std::cout << std::flush;
}

int main(int argc, char *argv[])
{
    // Prepare commandline options
    cxxopts::Options options(argv[0], "XCS Classifier System");
    AddOptions(options);

    // Parse commandline options
    const auto parsedOptions = options.parse(argc, argv);

    // XCS Hyperparameters
    const XCSParams params = ParseXCSParams(parsedOptions);
    OutputXCSParams(params);

    // Initialize experiment helper
    const ExperimentSettings settings = ParseExperimentSettings(parsedOptions);
    ExperimentHelper experimentHelper(settings);

    if (parsedOptions.count("mux"))
    {
        // Multiplexer problem
        const auto & env = experimentHelper.constructTrainEnv<MultiplexerEnvironment>(parsedOptions["mux"].as<int>(), parsedOptions["mux-i"].as<unsigned int>());
        experimentHelper.constructTestEnv<MultiplexerEnvironment>(parsedOptions["mux"].as<int>());

        experimentHelper.constructSystem<XCS>(env.availableActions(), params);

        RunExperiment(experimentHelper, parsedOptions["iter"].as<std::uint64_t>(), parsedOptions["condense-iter"].as<std::uint64_t>());
    }
    else if (parsedOptions.count("parity"))
    {
        // Even-parity problem
        const auto & env = experimentHelper.constructTrainEnv<EvenParityEnvironment>(parsedOptions["parity"].as<int>());
        experimentHelper.constructTestEnv<EvenParityEnvironment>(parsedOptions["parity"].as<int>());

        experimentHelper.constructSystem<XCS>(env.availableActions(), params);

        RunExperiment(experimentHelper, parsedOptions["iter"].as<std::uint64_t>(), parsedOptions["condense-iter"].as<std::uint64_t>());
    }
    else if (parsedOptions.count("majority"))
    {
        // Majority-on problem
        const auto & env = experimentHelper.constructTrainEnv<MajorityOnEnvironment>(parsedOptions["majority"].as<int>());
        experimentHelper.constructTestEnv<MajorityOnEnvironment>(parsedOptions["majority"].as<int>());

        experimentHelper.constructSystem<XCS>(env.availableActions(), params);

        RunExperiment(experimentHelper, parsedOptions["iter"].as<std::uint64_t>(), parsedOptions["condense-iter"].as<std::uint64_t>());
    }
    else if (parsedOptions.count("blc"))
    {
        // Block world problem
        const auto & trainEnv = experimentHelper.constructTrainEnv<BlockWorldEnvironment>(parsedOptions["blc"].as<std::string>(), parsedOptions["max-step"].as<uint64_t>(), parsedOptions["blc-3bit"].as<bool>(), parsedOptions["blc-diag"].as<bool>());
        const auto & testEnv = experimentHelper.constructTestEnv<BlockWorldEnvironment>(parsedOptions["blc"].as<std::string>(), parsedOptions["max-step"].as<uint64_t>(), parsedOptions["blc-3bit"].as<bool>(), parsedOptions["blc-diag"].as<bool>());

        auto & xcs = experimentHelper.constructSystem<XCS>(trainEnv.availableActions(), params);

        // Prepare trace output
        std::ofstream traceLogStream;
        bool outputTraceLog = !parsedOptions["blc-output-trace"].as<std::string>().empty();
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

        RunExperiment(experimentHelper, parsedOptions["iter"].as<std::uint64_t>(), parsedOptions["condense-iter"].as<std::uint64_t>());

        // Output best action map
        if (!parsedOptions["blc-output-best"].as<std::string>().empty())
        {
            std::ofstream ofs(parsedOptions["blc-output-best"].as<std::string>());

            bool useUnicode = parsedOptions["blc-output-best-uni"].as<bool>();

            for (int y = 0; y < testEnv.worldHeight(); ++y)
            {
                for (int x = 0; x < testEnv.worldWidth(); ++x)
                {
                    if (testEnv.isEmpty(x, y))
                    {
                        // Output the selected action
                        auto situation = testEnv.situation(x, y);
                        int action = xcs.exploit(situation);
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

        RunExperiment(experimentHelper, parsedOptions["iter"].as<std::uint64_t>(), parsedOptions["condense-iter"].as<std::uint64_t>());
    }
    else
    {
        std::cout << options.help({"", "Group"}) << std::endl;
        std::exit(parsedOptions.count("help") ? 0 : 1);
    }

    OutputPopulation(experimentHelper, settings.outputFilenamePrefix + parsedOptions["coutput"].as<std::string>());

    return 0;
}
