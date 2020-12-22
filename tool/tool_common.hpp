#pragma once
#include <cstdint> // std::uint64_t
#include <cxxopts.hpp>
#include <xcspp/xcspp.hpp>

namespace xcspp::tool_common
{

    void AddExperimentOptions(cxxopts::Options & options);

    ExperimentSettings ParseExperimentSettings(const cxxopts::ParseResult & parsedOptions);

    void OutputPopulation(const ExperimentHelper & experimentHelper, const std::string & filename);

    void RunExperiment(ExperimentHelper & experimentHelper, std::uint64_t iterationCount, std::uint64_t condensationIterationCount);

}
