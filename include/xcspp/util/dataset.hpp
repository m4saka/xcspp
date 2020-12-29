#pragma once
#include <vector>

namespace xcspp
{

    template <typename T>
    struct BasicDataset
    {
        static_assert(std::is_arithmetic_v<T>, "T of BasicDataset<T> must be an integer type or a floating-point type.");

        std::vector<std::vector<T>> situations;
        std::vector<int> actions;
    };

    using Dataset = BasicDataset<int>;
    using RealDataset = BasicDataset<double>;

}
