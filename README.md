# xcspp: XCS Library for C++17
## Usage (with CMake)
Just use `add_subdirectory` to include the xcspp directory, and link the `xcspp` library to your target.
```cmake
add_subdirectory(xcspp) # <= Path to the xcspp directory
target_link_libraries(your_target xcspp)
```

## Usage (without CMake)
Work in progress...

## Example: `XCS` class
```cpp
#include <iostream>
#include <xcspp/xcs.hpp>

int main()
{
    // XCS hyperparameters
    xcspp::XCSParams constants;
    constants.n = 1200; // N (max number of classifiers)

    // XCS classifier system
    xcspp::XCS xcs({ 0, 1 } /* Available actions */, constants);

    // Learning iterations
    for (int i = 0; i < 50000; ++i)
    {
        int action = xcs.explore(/* Input (std::vector<int>) */);

        // ... (Evaluate the action here!)

        xcs.reward(/* Reward (int) */);
    }

    // Show acquired rules
    xcs.dumpPopulation(std::cout);

    // Use "xcs.exploit(input)" to get the most desirable action
    // chosen by using acquired rules

    return 0;
}
```

## Example: `ExperimentHelper` class
For research purposes, you can use `ExperimentHelper` class to evaluate XCS performance and output log files with a simple code.

### Sample code:
```cpp
#include "xcspp/helper/experiment_helper.hpp"
#include "xcspp/environment/multiplexer_environment.hpp"

int main()
{
    // Experimental settings
    xcspp::ExperimentSettings settings;
    settings.outputSummaryToStdout = true; // This option enables to output summary log to console.

    // XCS hyperparameters
    xcspp::XCSParams constants;
    constants.n = 1200; // N (max number of classifiers)

    // Initialize experiment and environment
    ExperimentHelper helper(settings, constants);
    helper.constructEnvironments<xcspp::MultiplexerEnvironment>(6); // This calls xcspp::MultiplexerEnvironment constructor.

    // Run experiment
    helper.runIteration(50000);

    return 0;
}
```

### Output example ("Reward" is the classification accuracy):
```
Iteration      Reward      SysErr     PopSize  CovOccRate   TotalStep
========== =========== =========== =========== =========== ===========
     5000     673.400     420.729     275.018  0.01480000       1.000
    10000     783.400     359.733     241.782  0.00000000       1.000
    15000     939.600     157.625     164.181  0.00000000       1.000
    20000     982.800      50.831     110.646  0.00000000       1.000
    25000     999.200      20.434      89.697  0.00000000       1.000
    30000    1000.000       9.507      70.050  0.00000000       1.000
    35000    1000.000       8.502      69.459  0.00000000       1.000
    40000    1000.000      10.444      74.861  0.00000000       1.000
    45000    1000.000       8.563      70.950  0.00000000       1.000
    50000    1000.000       8.952      71.377  0.00000000       1.000
```
In default, this summary log is saved as `summary.csv`, and the acquired population is saved as `classifier.csv`.
