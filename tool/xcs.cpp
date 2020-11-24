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

using namespace xcspp;

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
    settings.useInputClassifierToResume = parsedOptions["resume"].as<bool>();
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
        ss << "             tau = " << params.tau << '\n';
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
    // Parse command line arguments
    cxxopts::Options options(argv[0], "XCS Classifier System");

    const XCSParams defaultParams;
    options
        //.allow_unrecognised_options()
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
        ("resume", "Whether to use initial classifiers (--cinput) to resume previous experiment (\"false\": initialize p/epsilon/F/exp/ts/as to defaults, \"true\": do not initialize values and set system time stamp to the same as that of the latest classifier)", cxxopts::value<bool>()->default_value("false"), "true/false")
        ("m,mux", "Use the multiplexer problem", cxxopts::value<int>(), "LENGTH")
        ("mux-i", "Class imbalance level i of the multiplexer problem (used only in train iterations)", cxxopts::value<unsigned int>()->default_value("0"), "LEVEL")
        //("parity", "Use the even-parity problem", cxxopts::value<int>(), "LENGTH")
        //("majority", "Use the majority-on problem", cxxopts::value<int>(), "LENGTH")
        //("blc", "Use the block world problem", cxxopts::value<std::string>(), "FILENAME")
        //("blc-3bit", "Use 3-bit representation for each block in a situation", cxxopts::value<bool>()->default_value("false"), "true/false")
        //("blc-diag", "Allow diagonal actions in the block world problem", cxxopts::value<bool>()->default_value("true"), "true/false")
        //("blc-output-best", "Output the parsedOptions of the desired action for blocks in the block world problem", cxxopts::value<std::string>()->default_value(""), "FILENAME")
        //("blc-output-best-uni", "Use UTF-8 square & arrow characters for --blc-output", cxxopts::value<bool>()->default_value("false"), "true/false")
        //("blc-output-trace", "Output the coordinate of the animat in the block world problem", cxxopts::value<std::string>()->default_value(""), "FILENAME")
        ("c,csv", "The csv file to train", cxxopts::value<std::string>(), "FILENAME")
        ("csv-test", "The csv file to test", cxxopts::value<std::string>(), "FILENAME")
        ("csv-random", "Whether to choose lines in random order from the csv file", cxxopts::value<bool>()->default_value("true"), "true/false")
        ("csv-estimate", "The csv file to estimate the outputs", cxxopts::value<std::string>(), "FILENAME")
        ("csv-output-best", "Output the parsedOptions of the desired action for the situations in the csv file specified by --csv-estimate", cxxopts::value<std::string>(), "FILENAME")
        ("max-step", "The maximum number of steps in the multi-step problem", cxxopts::value<uint64_t>()->default_value("50"))
        ("i,iter", "The number of iterations", cxxopts::value<uint64_t>()->default_value("20000"), "COUNT")
        ("condense-iter", "The number of iterations for the Wilson's rule condensation method (chi=0, mu=0) after normal iterations", cxxopts::value<uint64_t>()->default_value("0"), "COUNT")
        //("avg-seeds", "The number of different random seeds for averaging the reward and the macro-classifier count", cxxopts::value<uint64_t>()->default_value("1"), "COUNT")
        ("explore", "The number of exploration performed in each train iteration", cxxopts::value<uint64_t>()->default_value("1"), "COUNT")
        ("exploit", "The number of exploitation (= test mode) performed in each test iteration (set \"0\" if you don't need evaluation)", cxxopts::value<uint64_t>()->default_value("1"), "COUNT")
        ("exploit-upd", "Whether to update classifier parameters in test mode (\"auto\": false for single-step & true for multi-step)", cxxopts::value<std::string>()->default_value("auto"), "auto/true/false")
        ("sma", "The width of the simple moving average for the reward log", cxxopts::value<uint64_t>()->default_value("1"), "COUNT")
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

    const auto parsedOptions = options.parse(argc, argv);

    const bool isEnvironmentSpecified = (parsedOptions.count("mux") || /*parsedOptions.count("parity") || parsedOptions.count("majority") || parsedOptions.count("blc") ||*/ parsedOptions.count("csv"));

    // XCS Hyperparameters
    const XCSParams params = ParseXCSParams(parsedOptions);

    // Show help
    if (parsedOptions.count("help") || !isEnvironmentSpecified)
    {
        std::cout << options.help({"", "Group"}) << std::endl;
        std::exit(0);
    }

    OutputXCSParams(params);

    // Initialize experiment helper
    const ExperimentSettings settings = ParseExperimentSettings(parsedOptions);
    ExperimentHelper experimentHelper(settings);

    // Use multiplexer problem
    if (parsedOptions.count("mux"))
    {
        const auto & env = experimentHelper.constructTrainEnv<MultiplexerEnvironment>(parsedOptions["mux"].as<int>());
        experimentHelper.constructTestEnv<MultiplexerEnvironment>(parsedOptions["mux"].as<int>());

        experimentHelper.constructSystem<XCS>(env.availableActions(), params);
    }

    // Use csv file
    if (parsedOptions.count("csv"))
    {
        const std::string trainFilename = parsedOptions["csv"].as<std::string>();
        const std::string testFilename = parsedOptions.count("csv-eval") ? parsedOptions["csv-eval"].as<std::string>() : trainFilename;

        const auto & env = experimentHelper.constructTrainEnv<DatasetEnvironment>(CSV::ReadDatasetFromFile(trainFilename), parsedOptions["csv-random"].as<bool>());
        experimentHelper.constructTestEnv<DatasetEnvironment>(CSV::ReadDatasetFromFile(testFilename), parsedOptions["csv-random"].as<bool>());

        experimentHelper.constructSystem<XCS>(env.availableActions(), params);
    }

    RunExperiment(experimentHelper, parsedOptions["iter"].as<std::uint64_t>(), parsedOptions["condense-iter"].as<std::uint64_t>());

    OutputPopulation(experimentHelper, settings.outputFilenamePrefix + parsedOptions["coutput"].as<std::string>());

    return 0;
}
