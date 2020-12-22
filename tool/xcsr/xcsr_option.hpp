#pragma once
#include <cxxopts.hpp>
#include <xcspp/xcspp.hpp>

namespace xcspp::tool::xcsr
{

    void AddEnvironmentOptions(cxxopts::Options & options);

    void AddXCSROptions(cxxopts::Options & options);

    void AddOptions(cxxopts::Options & options);

    XCSRParams ParseXCSRParams(const cxxopts::ParseResult & parsedOptions);

    void OutputXCSRParams(const XCSRParams & params);

}
