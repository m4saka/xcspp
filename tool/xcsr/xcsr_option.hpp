#pragma once
#include <cxxopts.hpp>
#include <xcspp/xcspp.hpp>

namespace xcspp::tool::xcs
{

    void AddEnvironmentOptions(cxxopts::Options & options);

    void AddXCSOptions(cxxopts::Options & options);

    void AddOptions(cxxopts::Options & options);

    XCSParams ParseXCSParams(const cxxopts::ParseResult & parsedOptions);

    void OutputXCSParams(const XCSParams & params);

}
