#pragma once
#include <vector>

namespace xcspp
{

    struct Dataset
    {
        std::vector<std::vector<int>> situations;
        std::vector<int> actions;
    };

}
