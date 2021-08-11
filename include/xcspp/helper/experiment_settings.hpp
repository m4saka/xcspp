#pragma once
#include <string>
#include <cstddef> // std::size_t

namespace xcspp
{

    struct ExperimentSettings
    {
        // The number of exploration performed in each train iteration
        std::size_t explorationRepeat = 1;

        // The number of exploitation (= test mode) performed in each test iteration (set "0" if you don't need evaluation)
        std::size_t exploitationRepeat = 1;

        // Whether to update classifier parameters in test mode (usually "false" for single-step problems and "true" for multi-step problems)
        bool updateInExploitation = false;

        // The iteration interval of average log output
        std::size_t summaryInterval = 5000;

        // The prefix of filename
        std::string outputFilenamePrefix = "";

        // Whether to output summary log to stdout
        bool outputSummaryToStdout = false;

        // The filename of summary log csv output
        std::string outputSummaryFilename = "";

        // The filename of reward log csv output
        std::string outputRewardFilename = "";

        // The filename of macro-classifier count log csv output
        std::string outputPopulationSizeFilename = "";

        // The filename of system error log csv output
        std::string outputSystemErrorFilename = "";

        // The filename of number-of-step log csv output in multi-step problems
        std::string outputStepCountFilename = "";

        // The classifier csv filename for initial population
        std::string inputClassifierFilename = "";

        // Whether to use initial classifiers (--cinput) to resume previous experiment
        //   "false": initialize p/epsilon/F/exp/ts/as to defaults
        //   "true": do not initialize values and set system time stamp to the same as that of the latest classifier
        bool useInputClassifierToResume = true;

        // The width of the simple moving average for the reward log
        std::size_t smaWidth = 1;
    };

}
