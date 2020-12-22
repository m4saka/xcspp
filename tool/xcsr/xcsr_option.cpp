#include "xcsr_option.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <cstdint> // std::uint64_t
#include "common/common.hpp"

namespace xcspp::tool::xcsr
{

    void AddEnvironmentOptions(cxxopts::Options & options)
    {
        options.add_options("Environment")
            ("m,rmux", "Use the real multiplexer problem", cxxopts::value<int>(), "LENGTH")
            ("rmux-i", "Class imbalance level i of the multiplexer problem (used only in train iterations)", cxxopts::value<unsigned int>()->default_value("0"), "LEVEL");
            //("c,csv", "The csv file to train", cxxopts::value<std::string>(), "FILENAME")
            //("csv-test", "The csv file to test", cxxopts::value<std::string>(), "FILENAME")
            //("csv-random", "Whether to choose lines in random order from the csv file", cxxopts::value<bool>()->default_value("true"), "true/false")
            ////("csv-estimate", "The csv file to estimate the outputs", cxxopts::value<std::string>(), "FILENAME")
            ////("csv-output-best", "Output the parsedOptions of the desired action for the situations in the csv file specified by --csv-estimate", cxxopts::value<std::string>(), "FILENAME")
            //("max-step", "The maximum number of steps (teletransportation) in multi-step problems", cxxopts::value<std::uint64_t>()->default_value("50"), "STEP")
    }

    void AddXCSROptions(cxxopts::Options & options)
    {
        const XCSRParams defaultParams;
        options.add_options("XCSR parameter")
            ("repr", "The XCSR representation (Center-Spread: 'csr' / Lower-Upper [Ordered Bound]: 'obr' / Unordered Bound: 'ubr')", cxxopts::value<std::string>()->default_value("csr"), "csr/obr/ubr")
            ("N,max-population", "The maximum size of the population", cxxopts::value<std::uint64_t>()->default_value(std::to_string(defaultParams.n)), "SIZE")
            ("s-0", "The maximum value of a spread in the covering operator", cxxopts::value<double>()->default_value(std::to_string(defaultParams.s0)), "S_0")
            ("max-mutation", "The maximum change of a spread value or a center value in mutation", cxxopts::value<double>()->default_value(std::to_string(defaultParams.m)), "M")
            ("min-value", "The minimum value of each symbol in a situation", cxxopts::value<double>()->default_value(std::to_string(defaultParams.minValue)), "VALUE")
            ("max-value", "The maximum value of each symbol in a situation", cxxopts::value<double>()->default_value(std::to_string(defaultParams.maxValue)), "VALUE")
            ("alpha", "The fall of rate in the fitness evaluation", cxxopts::value<double>()->default_value(std::to_string(defaultParams.alpha)), "ALPHA")
            ("beta", "The learning rate for updating fitness, prediction, prediction error, and action set size estimate in XCS's classifiers", cxxopts::value<double>()->default_value(std::to_string(defaultParams.beta)), "BETA")
            ("epsilon-0", "The error threshold under which the accuracy of a classifier is set to one", cxxopts::value<double>()->default_value(std::to_string(defaultParams.alpha)), "EPSILON_0")
            ("nu", "The exponent in the power function for the fitness evaluation", cxxopts::value<double>()->default_value(std::to_string(defaultParams.nu)), "NU")
            ("gamma", "The discount rate in multi-step problems", cxxopts::value<double>()->default_value(std::to_string(defaultParams.gamma)), "GAMMA")
            ("theta-ga", "The threshold for the GA application in an action set", cxxopts::value<std::uint64_t>()->default_value(std::to_string(defaultParams.thetaGA)), "THETA_GA")
            ("chi", "The probability of applying crossover", cxxopts::value<double>()->default_value(std::to_string(defaultParams.chi)), "CHI")
            ("x-method", "The method of crossover operator", cxxopts::value<std::string>()->default_value("uniform"), "uniform/one-point/two-point")
            ("mu", "The probability of mutating one allele and the action", cxxopts::value<double>()->default_value(std::to_string(defaultParams.mu)), "MU")
            ("theta-del", "The experience threshold over which the fitness of a classifier may be considered in its deletion probability", cxxopts::value<double>()->default_value(std::to_string(defaultParams.thetaDel)), "THETA_DEL")
            ("delta", "The fraction of the mean fitness of the population below which the fitness of a classifier may be considered in its vote for deletion", cxxopts::value<double>()->default_value(std::to_string(defaultParams.delta)), "DELTA")
            ("theta-sub", "The experience of a classifier required to be a subsumer", cxxopts::value<double>()->default_value(std::to_string(defaultParams.thetaSub)), "THETA_SUB")
            ("tau", "The tournament size for selection [Butz et al., 2003] (set \"0\" to use the roulette-wheel selection)", cxxopts::value<double>()->default_value(std::to_string(defaultParams.tau)), "TAU")
            ("p-i", "The initial prediction value when generating a new classifier", cxxopts::value<double>()->default_value(std::to_string(defaultParams.initialPrediction)), "P_I")
            ("epsilon-i", "The initial prediction error value when generating a new classifier", cxxopts::value<double>()->default_value(std::to_string(defaultParams.initialEpsilon)), "EPSILON_I")
            ("f-i", "The initial fitness value when generating a new classifier", cxxopts::value<double>()->default_value(std::to_string(defaultParams.initialFitness)), "F_I")
            ("p-explr", "The probability during action selection of choosing the action uniform randomly", cxxopts::value<double>()->default_value(std::to_string(defaultParams.exploreProbability)), "P_EXPLR")
            ("theta-mna", "The minimal number of actions that must be present in a match set [M], or else covering will occur. Use \"0\" to set automatically.", cxxopts::value<std::uint64_t>()->default_value(std::to_string(defaultParams.thetaMna)), "THETA_MNA")
            ("do-ga-subsumption", "Whether offspring are to be tested for possible logical subsumption by parents", cxxopts::value<bool>()->default_value(defaultParams.doGASubsumption ? "true" : "false"), "true/false")
            ("do-as-subsumption", "Whether action sets are to be tested for subsuming classifiers", cxxopts::value<bool>()->default_value(defaultParams.doActionSetSubsumption ? "true" : "false"), "true/false")
            ("do-action-mutation", "Whether to apply mutation to the action", cxxopts::value<bool>()->default_value(defaultParams.doActionMutation ? "true" : "false"), "true/false")
            ("do-range-restriction", "Whether to restrict the range of the condition to the interval [min-value, max-value) in the covering and mutation operator (ignored when --repr=csr)", cxxopts::value<bool>()->default_value(defaultParams.doRangeRestriction ? "true" : "false"), "true/false")
            ("do-covering-random-range-truncation", "Whether to truncate the covering random range before generating random intervals if the interval [x-s_0, x+s_0) is not contained in [min-value, max-value).  \"false\" is common for this option, but the covering operator can generate too many maximum-range intervals if s_0 is larger than (max-value - min-value) / 2.  Choose \"true\" to avoid the random bias in this situation.  (ignored when --repr=csr)", cxxopts::value<bool>()->default_value(defaultParams.doCoveringRandomRangeTruncation ? "true" : "false"), "true/false")
            ("mam", "Whether to use the moyenne adaptive modifee (MAM) for updating the prediction and the prediction error of classifiers", cxxopts::value<bool>()->default_value(defaultParams.useMAM ? "true" : "false"), "true/false");
    }

    void AddOptions(cxxopts::Options & options)
    {
        tool::AddExperimentOptions(options);
        AddEnvironmentOptions(options);
        AddXCSROptions(options);
        options.add_options()
            ("h,help", "Show this help");
    }

    XCSRParams ParseXCSRParams(const cxxopts::ParseResult & parsedOptions)
    {
        XCSRParams params;
        params.n = parsedOptions["max-population"].as<std::uint64_t>();
        params.s0 = parsedOptions["s-0"].as<double>();
        params.m = parsedOptions["max-mutation"].as<double>();
        params.minValue = parsedOptions["min-value"].as<double>();
        params.maxValue = parsedOptions["max-value"].as<double>();
        params.alpha = parsedOptions["alpha"].as<double>();
        params.beta = parsedOptions["beta"].as<double>();
        params.epsilonZero = parsedOptions["epsilon-0"].as<double>();
        params.nu = parsedOptions["nu"].as<double>();
        params.gamma = parsedOptions["gamma"].as<double>();
        params.thetaGA = parsedOptions["theta-ga"].as<std::uint64_t>();
        params.chi = parsedOptions["chi"].as<double>();
        params.mu = parsedOptions["mu"].as<double>();
        params.thetaDel = parsedOptions["theta-del"].as<double>();
        params.delta = parsedOptions["delta"].as<double>();
        params.thetaSub = parsedOptions["theta-sub"].as<double>();
        params.tau = parsedOptions["tau"].as<double>();
        params.initialPrediction = parsedOptions["p-i"].as<double>();
        params.initialEpsilon = parsedOptions["epsilon-i"].as<double>();
        params.initialFitness = parsedOptions["f-i"].as<double>();
        params.exploreProbability = parsedOptions["p-explr"].as<double>();
        params.thetaMna = parsedOptions["theta-mna"].as<std::uint64_t>();
        params.doGASubsumption = parsedOptions["do-ga-subsumption"].as<bool>();
        params.doActionSetSubsumption = parsedOptions["do-as-subsumption"].as<bool>();
        params.doActionMutation = parsedOptions["do-action-mutation"].as<bool>();
        params.doRangeRestriction = parsedOptions["do-range-restriction"].as<bool>();
        params.doCoveringRandomRangeTruncation = parsedOptions["do-covering-random-range-truncation"].as<bool>();
        params.useMAM = parsedOptions["mam"].as<bool>();

        const std::string reprStr = parsedOptions["repr"].as<std::string>();
        if (reprStr == "csr")
        {
            params.repr = XCSRRepr::kCSR;
        }
        else if (reprStr == "obr")
        {
            params.repr = XCSRRepr::kOBR;
        }
        else if (reprStr == "ubr")
        {
            params.repr = XCSRRepr::kUBR;
        }
        else
        {
            std::cerr << "Error: Unknown value for --repr (" << reprStr << ")" << std::endl;
            std::exit(1);
        }

        // Determine crossover method
        if (parsedOptions["x-method"].as<std::string>() == "uniform")
        {
            params.crossoverMethod = XCSRParams::CrossoverMethod::kUniformCrossover;
        }
        else if (parsedOptions["x-method"].as<std::string>() == "one-point")
        {
            params.crossoverMethod = XCSRParams::CrossoverMethod::kOnePointCrossover;
        }
        else if (parsedOptions["x-method"].as<std::string>() == "two-point")
        {
            params.crossoverMethod = XCSRParams::CrossoverMethod::kTwoPointCrossover;
        }
        else
        {
            std::cerr << "Error: Unknown value for --x-method (" << parsedOptions["x-method"].as<std::string>() << ")" << std::endl;
            std::exit(1);
        }

        return params;
    }

    void OutputXCSRParams(const XCSRParams & params)
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
        case XCSRParams::CrossoverMethod::kUniformCrossover:
            std::cout << "uniform\n";
            break;
        case XCSRParams::CrossoverMethod::kOnePointCrossover:
            std::cout << "one-point\n";
            break;
        case XCSRParams::CrossoverMethod::kTwoPointCrossover:
            std::cout << "two-point\n";
            break;
        }
        std::cout << '\n';

        std::cout << "[ XCSR Parameters ]\n";
        std::cout << "  representation = ";
        switch (params.repr)
        {
        case XCSRRepr::kCSR:
            std::cout << "CSR (Center-Spread)\n";
            break;
        case XCSRRepr::kOBR:
            std::cout << "OBR (Lower-Upper)\n";
            break;
        case XCSRRepr::kUBR:
            std::cout << "UBR (Unordered Bound)\n";
            break;
        }
        std::cout << "             s_0 = " << params.s0 << '\n';
        std::cout << "               m = " << params.m << '\n';
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
            ss << "             MAM = false\n";
        const std::string str = ss.str();
        if (!str.empty())
        {
            std::cout << "[ XCSR Optional Settings ]\n";
            std::cout << str << '\n';
        }

        std::cout << std::flush;
    }

}
