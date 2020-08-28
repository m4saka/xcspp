#pragma once
#include <vector>

namespace xcscpp
{

    struct Dataset
    {
        std::vector<std::vector<int>> situations;
        std::vector<int> actions;
    };

}
