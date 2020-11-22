#pragma once
#include <vector>

namespace xcspp
{

    template <typename T>
    struct BasicDataset
    {
        std::vector<std::vector<T>> situations;
        std::vector<T> actions;
    };

    using Dataset = BasicDataset<int>;

}
