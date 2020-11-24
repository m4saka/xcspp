#define __USE_MINGW_ANSI_STDIO 0
#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <unordered_set>
#include <cstddef>

#include <xcspp/xcspp.hpp>
#include <cxxopts.hpp>

using namespace xcspp;

int main(int argc, char *argv[])
{
    XCSParams params;

    // Parse command line arguments
    cxxopts::Options options(argv[0], "XCS Classifier System");

    options
        .allow_unrecognised_options()
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
        ("blc", "Use the block world problem", cxxopts::value<std::string>(), "FILENAME")
        ("blc-3bit", "Use 3-bit representation for each block in a situation", cxxopts::value<bool>()->default_value("false"), "true/false")
        ("blc-diag", "Allow diagonal actions in the block world problem", cxxopts::value<bool>()->default_value("true"), "true/false")
        ("blc-output-best", "Output the result of the desired action for blocks in the block world problem", cxxopts::value<std::string>()->default_value(""), "FILENAME")
        ("blc-output-best-uni", "Use UTF-8 square & arrow characters for --blc-output", cxxopts::value<bool>()->default_value("false"), "true/false")
        ("blc-output-trace", "Output the coordinate of the animat in the block world problem", cxxopts::value<std::string>()->default_value(""), "FILENAME")
        ("c,csv", "The csv file to train", cxxopts::value<std::string>(), "FILENAME")
        ("csv-test", "The csv file to test", cxxopts::value<std::string>(), "FILENAME")
        ("csv-random", "Whether to choose lines in random order from the csv file", cxxopts::value<bool>()->default_value("true"), "true/false")
        ("csv-estimate", "The csv file to estimate the outputs", cxxopts::value<std::string>(), "FILENAME")
        ("csv-output-best", "Output the result of the desired action for the situations in the csv file specified by --csv-estimate", cxxopts::value<std::string>(), "FILENAME")
        ("max-step", "The maximum number of steps in the multi-step problem", cxxopts::value<uint64_t>()->default_value("50"))
        ("i,iter", "The number of iterations", cxxopts::value<uint64_t>()->default_value("20000"), "COUNT")
        ("condense-iter", "The number of iterations for the Wilson's rule condensation method (chi=0, mu=0) after normal iterations", cxxopts::value<uint64_t>()->default_value("0"), "COUNT")
        //("avg-seeds", "The number of different random seeds for averaging the reward and the macro-classifier count", cxxopts::value<uint64_t>()->default_value("1"), "COUNT")
        ("explore", "The number of exploration performed in each train iteration", cxxopts::value<uint64_t>()->default_value("1"), "COUNT")
        ("exploit", "The number of exploitation (= test mode) performed in each test iteration (set \"0\" if you don't need evaluation)", cxxopts::value<uint64_t>()->default_value("1"), "COUNT")
        ("exploit-upd", "Whether to update classifier parameters in test mode (\"auto\": false for single-step & true for multi-step)", cxxopts::value<std::string>()->default_value("auto"), "auto/true/false")
        ("sma", "The width of the simple moving average for the reward log", cxxopts::value<uint64_t>()->default_value("1"), "COUNT")
        ("a,action", "The available action choices for csv (comma-separated, integer only)", cxxopts::value<std::string>(), "ACTIONS")
        ("N,max-population", "The maximum size of the population", cxxopts::value<uint64_t>()->default_value(std::to_string(params.n)), "COUNT")
        ("alpha", "The fall of rate in the fitness evaluation", cxxopts::value<double>()->default_value(std::to_string(params.alpha)), "ALPHA")
        ("beta", "The learning rate for updating fitness, prediction, prediction error, and action set size estimate in XCS's classifiers", cxxopts::value<double>()->default_value(std::to_string(params.beta)), "BETA")
        ("epsilon-0", "The error threshold under which the accuracy of a classifier is set to one", cxxopts::value<double>()->default_value(std::to_string(params.alpha)), "EPSILON_0")
        ("nu", "The exponent in the power function for the fitness evaluation", cxxopts::value<double>()->default_value(std::to_string(params.nu)), "NU")
        ("gamma", "The discount rate in multi-step problems", cxxopts::value<double>()->default_value(std::to_string(params.gamma)), "GAMMA")
        ("theta-ga", "The threshold for the GA application in an action set", cxxopts::value<uint64_t>()->default_value(std::to_string(params.thetaGA)), "THETA_GA")
        ("chi", "The probability of applying crossover", cxxopts::value<double>()->default_value(std::to_string(params.chi)), "CHI")
        ("x-method", "The method of crossover operator", cxxopts::value<std::string>()->default_value("uniform"), "uniform/one-point/two-point")
        ("mu", "The probability of mutating one allele and the action", cxxopts::value<double>()->default_value(std::to_string(params.mu)), "MU")
        ("theta-del", "The experience threshold over which the fitness of a classifier may be considered in its deletion probability", cxxopts::value<double>()->default_value(std::to_string(params.thetaDel)), "THETA_DEL")
        ("delta", "The fraction of the mean fitness of the population below which the fitness of a classifier may be considered in its vote for deletion", cxxopts::value<double>()->default_value(std::to_string(params.delta)), "DELTA")
        ("theta-sub", "The experience of a classifier required to be a subsumer", cxxopts::value<double>()->default_value(std::to_string(params.thetaSub)), "THETA_SUB")
        ("tau", "The tournament size for selection [Butz et al., 2003] (set \"0\" to use the roulette-wheel selection)", cxxopts::value<double>()->default_value(std::to_string(params.tau)), "TAU")
        ("s,p-sharp", "The probability of using a don't care symbol in an allele when covering", cxxopts::value<double>()->default_value(std::to_string(params.dontCareProbability)), "P_SHARP")
        ("initial-prediction", "The initial prediction value when generating a new classifier", cxxopts::value<double>()->default_value(std::to_string(params.initialPrediction)), "P_I")
        ("initial-prediction-error", "The initial prediction error value when generating a new classifier", cxxopts::value<double>()->default_value(std::to_string(params.initialEpsilon)), "EPSILON_I")
        ("initial-fitness", "The initial fitness value when generating a new classifier", cxxopts::value<double>()->default_value(std::to_string(params.initialFitness)), "F_I")
        ("p-explr", "The probability during action selection of choosing the action uniform randomly", cxxopts::value<double>()->default_value(std::to_string(params.exploreProbability)), "P_EXPLR")
        ("theta-mna", "The minimal number of actions that must be present in a match set [M], or else covering will occur. Use \"0\" to set automatically.", cxxopts::value<uint64_t>()->default_value(std::to_string(params.thetaMna)), "THETA_MNA")
        ("do-ga-subsumption", "Whether offspring are to be tested for possible logical subsumption by parents", cxxopts::value<bool>()->default_value(params.doGASubsumption ? "true" : "false"), "true/false")
        ("do-action-set-subsumption", "Whether action sets are to be tested for subsuming classifiers", cxxopts::value<bool>()->default_value(params.doActionSetSubsumption ? "true" : "false"), "true/false")
        ("do-action-mutation", "Whether to apply mutation to the action", cxxopts::value<bool>()->default_value(params.doActionMutation ? "true" : "false"), "true/false")
        ("mam", "Whether to use the moyenne adaptive modifee (MAM) for updating the prediction and the prediction error of classifiers", cxxopts::value<bool>()->default_value(params.useMAM ? "true" : "false"), "true/false")
        ("h,help", "Show this help");

    auto result = options.parse(argc, argv);

    // Set hyperparameters
    if (result.count("max-population"))
        params.n = result["max-population"].as<uint64_t>();
    if (result.count("alpha"))
        params.alpha = result["alpha"].as<double>();
    if (result.count("beta"))
        params.beta = result["beta"].as<double>();
    if (result.count("epsilon-0"))
        params.epsilonZero = result["epsilon-0"].as<double>();
    if (result.count("nu"))
        params.nu = result["nu"].as<double>();
    if (result.count("gamma"))
        params.gamma = result["gamma"].as<double>();
    if (result.count("theta-ga"))
        params.thetaGA = result["theta-ga"].as<uint64_t>();
    if (result.count("chi"))
        params.chi = result["chi"].as<double>();
    if (result.count("mu"))
        params.mu = result["mu"].as<double>();
    if (result.count("theta-del"))
        params.thetaDel = result["theta-del"].as<double>();
    if (result.count("delta"))
        params.delta = result["delta"].as<double>();
    if (result.count("theta-sub"))
        params.thetaSub = result["theta-sub"].as<double>();
    if (result.count("tau"))
        params.tau = result["tau"].as<double>();
    if (result.count("p-sharp"))
        params.dontCareProbability = result["p-sharp"].as<double>();
    if (result.count("initial-prediction"))
        params.initialPrediction = result["initial-prediction"].as<double>();
    if (result.count("initial-prediction-error"))
        params.initialEpsilon = result["initial-prediction-error"].as<double>();
    if (result.count("initial-fitness"))
        params.initialFitness = result["initial-fitness"].as<double>();
    if (result.count("p-explr"))
        params.exploreProbability = result["p-explr"].as<double>();
    if (result.count("theta-mna"))
        params.thetaMna = result["theta-mna"].as<uint64_t>();
    if (result.count("do-ga-subsumption"))
        params.doGASubsumption = result["do-ga-subsumption"].as<bool>();
    if (result.count("do-action-set-subsumption"))
        params.doActionSetSubsumption = result["do-action-set-subsumption"].as<bool>();
    if (result.count("do-action-mutation"))
        params.doActionMutation = result["do-action-mutation"].as<bool>();
    if (result.count("mam"))
        params.useMAM = result["mam"].as<bool>();

    const bool isEnvironmentSpecified = (result.count("mux") || /*result.count("parity") || result.count("majority") || result.count("blc") ||*/ result.count("csv"));
    if (!isEnvironmentSpecified)
    {
        // No target environment (show help)
        std::cout << options.help({"", "Group"}) << std::endl;
        return 1;
    }

    // Determine crossover method
    if (result["x-method"].as<std::string>() == "uniform")
    {
        params.crossoverMethod = XCSParams::CrossoverMethod::UNIFORM_CROSSOVER;
    }
    else if (result["x-method"].as<std::string>() == "one-point")
    {
        params.crossoverMethod = XCSParams::CrossoverMethod::ONE_POINT_CROSSOVER;
    }
    else if (result["x-method"].as<std::string>() == "two-point")
    {
        params.crossoverMethod = XCSParams::CrossoverMethod::TWO_POINT_CROSSOVER;
    }
    else
    {
        std::cerr << "Error: Unknown value for --x-method (" << result["x-method"].as<std::string>() << ")" << std::endl;
        exit(1);
    }

    // Determine whether to update classifier parameters in exploitation
    bool updateInExploitation;
    if (result["exploit-upd"].as<std::string>() == "true")
    {
        updateInExploitation = true;
    }
    else if (result["exploit-upd"].as<std::string>() == "false")
    {
        updateInExploitation = false;
    }
    else if (result["exploit-upd"].as<std::string>() == "auto")
    {
        updateInExploitation = result.count("blc");
    }
    else
    {
        std::cerr << "Error: Unknown value for --exploit-upd (" << result["exploit-upd"].as<std::string>() << ")" << std::endl;
        exit(1);
    }

    // Show help
    if (result.count("help") || !isEnvironmentSpecified)
    {
        std::cout << options.help({"", "Group"}) << std::endl;
        exit(0);
    }
    else
    {
        // Output parameters
        std::cout << "[ XCS General Parameters ]" << std::endl;
        std::cout << "               N = " << params.n << std::endl;
        std::cout << "            beta = " << params.beta << std::endl;
        std::cout << "           alpha = " << params.alpha << std::endl;
        std::cout << "       epsilon_0 = " << params.epsilonZero << std::endl;
        std::cout << "              nu = " << params.nu << std::endl;
        std::cout << "           gamma = " << params.gamma << std::endl;
        std::cout << "        theta_GA = " << params.thetaGA << std::endl;
        std::cout << "             chi = " << params.chi << std::endl;
        std::cout << "              mu = " << params.mu << std::endl;
        std::cout << "       theta_del = " << params.thetaDel << std::endl;
        std::cout << "           delta = " << params.delta << std::endl;
        std::cout << "       theta_sub = " << params.thetaSub << std::endl;
        std::cout << "             P_# = " << params.dontCareProbability << std::endl;
        std::cout << "             p_I = " << params.initialPrediction << std::endl;
        std::cout << "       epsilon_I = " << params.initialEpsilon << std::endl;
        std::cout << "             F_I = " << params.initialFitness << std::endl;
        std::cout << "         p_explr = " << params.exploreProbability << std::endl;
        if (params.thetaMna != 0) std::cout << "       theta_mna = " << params.thetaMna << std::endl;
        std::cout << " doGASubsumption = " << (params.doGASubsumption ? "true" : "false") << std::endl;
        std::cout << " doASSubsumption = " << (params.doActionSetSubsumption ? "true" : "false") << std::endl;
        std::cout << " crossoverMethod = ";
        switch (params.crossoverMethod)
        {
        case XCSParams::CrossoverMethod::UNIFORM_CROSSOVER:
            std::cout << "uniform" << std::endl;
            break;
        case XCSParams::CrossoverMethod::ONE_POINT_CROSSOVER:
            std::cout << "one-point" << std::endl;
            break;
        case XCSParams::CrossoverMethod::TWO_POINT_CROSSOVER:
            std::cout << "two-point" << std::endl;
            break;
        }
        std::cout << std::endl;

        // Output optional settings
        std::stringstream ss;
        if (params.tau > 0.0 && params.tau <= 1.0)
            ss << "             tau = " << params.tau << std::endl;
        if (!params.doActionMutation)
            ss << "doActionMutation = false" << std::endl;
        if (!params.useMAM)
            ss << "          useMAM = false" << std::endl;
        std::string str = ss.str();
        if (!str.empty())
        {
            std::cout << "[ XCS Optional Settings ]" << std::endl;
            std::cout << str << std::endl;
        }
    }

    ExperimentSettings settings;
    settings.explorationRepeat = result["explore"].as<uint64_t>();
    settings.exploitationRepeat = result["exploit"].as<uint64_t>();
    settings.updateInExploitation = updateInExploitation;
    settings.summaryInterval = result["summary-interval"].as<uint64_t>();
    settings.outputFilenamePrefix = result["prefix"].as<std::string>();
    settings.outputSummaryToStdout = true;
    settings.outputSummaryFilename = result["soutput"].as<std::string>();
    settings.outputRewardFilename = result["routput"].as<std::string>();
    settings.outputSystemErrorFilename = result["seoutput"].as<std::string>();
    settings.outputPopulationSizeFilename = result["noutput"].as<std::string>();
    settings.outputStepCountFilename = result["nsoutput"].as<std::string>();
    settings.inputClassifierFilename = result["cinput"].as<std::string>();
    settings.useInputClassifierToResume = result["resume"].as<bool>();
    settings.smaWidth = result["sma"].as<uint64_t>();

    ExperimentHelper experimentHelper(settings);

    // Use multiplexer problem
    if (result.count("mux"))
    {
        const auto & env = experimentHelper.constructTrainEnv<MultiplexerEnvironment>(result["mux"].as<int>());
        experimentHelper.constructTestEnv<MultiplexerEnvironment>(result["mux"].as<int>());

        experimentHelper.constructSystem<XCS>(env.availableActions(), params);
    }

    // Use csv file
    if (result.count("csv"))
    {
        // Get available action choices
        if (!result.count("action"))
        {
            std::cout << "Error: Available action list (--action) is not specified." << std::endl;
            exit(1);
        }
        const std::string availableActionsStr = result["action"].as<std::string>();
        std::string availableActionStr;
        std::stringstream ss(availableActionsStr);
        std::unordered_set<int> availableActions;
        while (std::getline(ss, availableActionStr, ','))
        {
            try
            {
                availableActions.insert(std::stoi(availableActionStr));
            }
            catch (std::exception & e)
            {
                std::cout << "Error: Action must be an integer." << std::endl;
                exit(1);
            }
        }

        const std::string trainFilename = result["csv"].as<std::string>();
        std::string testFilename = trainFilename;
        if (result.count("csv-eval"))
        {
            testFilename = result["csv-eval"].as<std::string>();
        }

        const auto & env = experimentHelper.constructTrainEnv<DatasetEnvironment>(CSV::ReadDatasetFromFile(trainFilename), result["csv-random"].as<bool>());
        experimentHelper.constructTestEnv<DatasetEnvironment>(CSV::ReadDatasetFromFile(testFilename), result["csv-random"].as<bool>());

        experimentHelper.constructSystem<XCS>(env.availableActions(), params);
    }

    // Run experiment
    const uint64_t iterationCount = result["iter"].as<uint64_t>();
    const uint64_t condensationIterationCount = result["condense-iter"].as<uint64_t>();

    experimentHelper.runIteration(iterationCount);
    experimentHelper.switchToCondensationMode();
    experimentHelper.runIteration(condensationIterationCount);

    // Save population
    {
        const std::string filename = settings.outputFilenamePrefix + result["coutput"].as<std::string>();

        std::ofstream ofs;
        std::ostream & os = filename.empty() ? std::cout : ofs;
        if (!filename.empty())
        {
            ofs.open(filename);
        }
        if (os)
        {
            experimentHelper.system().outputPopulationCSV(os);
        }
    }

    return 0;
}
