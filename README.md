# xcspp: XCS Library for C++17
eXtended Learning Classifier System (XCS) library written in C++17.

A minimal working example is available here: https://github.com/m4saka/xcspp-sample

![Ubuntu build](https://github.com/m4saka/xcspp/workflows/Ubuntu%20build/badge.svg)
![Windows build](https://github.com/m4saka/xcspp/workflows/Windows%20build/badge.svg)
![macOS build](https://github.com/m4saka/xcspp/workflows/macOS%20build/badge.svg)

## What is XCS?
XCS is a rule-based machine learning method proposed by Wilson [1].

It generates if-then rules (i.e., classifiers) by Genetic Algorithm, and updates them by a Q-learning-like reinforcement learning mechanism.  
Also, the method has an operation called subsumption, which integrates rules that are in an inclusion relationship with each other, and the smallest ruleset needed for classification is generated.

Technically, XCS is a variant of Michigan-style Learning Classifier Systems (LCS) that updates the fitness based on the accuracy of reward prediction.

Since the method employs a human-readable knowledge representation, it could be applied to tasks that require interpretability, such as data mining.

<sub>[1] S. W. Wilson, "Classifier Fitness Based on Accuracy," Evolutionary Computation, Vol.3, No.2, pp.149-175, June 1995. https://doi.org/10.1162/evco.1995.3.2.149</sub>

## `XCS` class
`XCS` class is suitable for general use of the XCS classifier system.  
You can use it to embed XCS in your program.

### Sample code:
```cpp
#include <iostream>
#include <xcspp/xcspp.hpp>

int main()
{
    // XCS hyperparameters
    xcspp::XCSParams params;
    params.n = 1200; // N (max number of classifiers)

    // XCS classifier system
    xcspp::XCS xcs({ 0, 1 } /* Available actions */, params);

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
- Note: In the LCS community, `explore`/`exploit` has a similar meaning to "train"/"test" used in ordinary machine learning.

## `XCSExperimentHelper` class
The `XCSExperimentHelper` class allows you to evaluate the performance of XCS with a simple code. 
This is useful especially for research purposes.

### Sample code (20-bit multiplexer problem):
```cpp
#include <xcspp/xcspp.hpp>

int main()
{
    // Experimental settings
    xcspp::ExperimentSettings settings;
    settings.outputSummaryToStdout = true; // This option enables to output summary log to console.

    // XCS hyperparameters
    xcspp::XCSParams params;
    params.n = 1200; // N (max number of classifiers)

    // Initialize experiment and environment
    xcspp::XCSExperimentHelper helper(settings, params);
    helper.constructEnvironments<xcspp::MultiplexerEnvironment>(20); // This calls xcspp::MultiplexerEnvironment constructor.

    // Run experiment
    helper.runIteration(50000);

    return 0;
}
```
- Note: If you want to use your own benchmark problem class with `XCSExperimentHelper`, it is required that your environment class implements `IEnvironment` interface declared in `include/xcspp/environment/ienvironment.hpp`.

### Output example ("Reward" is the classification accuracy):
```
  Iteration      Reward      SysErr     PopSize  CovOccRate   TotalStep
 ========== =========== =========== =========== =========== ===========
       5000     522.800     481.732    1056.961  0.27140000       1.000
      10000     561.800     481.459    1062.928  0.00000000       1.000
      15000     612.400     462.474    1039.645  0.00000000       1.000
      20000     654.600     443.171    1017.812  0.00000000       1.000
      25000     687.600     425.879     972.704  0.00000000       1.000
      30000     783.200     348.838     889.464  0.00000000       1.000
      35000     858.400     282.311     801.090  0.00000000       1.000
      40000     939.000     168.693     637.753  0.00000000       1.000
      45000     984.000      81.269     485.382  0.00000000       1.000
      50000     994.200      31.550     345.738  0.00000000       1.000
      55000     998.800      20.355     277.215  0.00000000       1.000
      60000     999.600      13.514     265.082  0.00000000       1.000
      65000    1000.000       9.880     241.245  0.00000000       1.000
      70000    1000.000      12.519     234.592  0.00000000       1.000
      75000    1000.000      14.274     226.879  0.00000000       1.000
      80000    1000.000      14.387     242.632  0.00000000       1.000
      85000    1000.000      10.195     223.938  0.00000000       1.000
      90000    1000.000      10.196     215.379  0.00000000       1.000
      95000    1000.000       9.190     215.330  0.00000000       1.000
     100000    1000.000       9.963     228.785  0.00000000       1.000
```
In default, this summary log is saved as `summary.csv`, and the acquired population is saved as `classifier.csv`.

## Library Linking (with CMake)
Just include the xcspp directory using `add_subdirectory` and link the `xcspp` library to the target.
```cmake
add_subdirectory(xcspp) # <= Path to the xcspp directory
target_link_libraries(your_target xcspp)
```

## Library Linking (without CMake)
Work in progress...
