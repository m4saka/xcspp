#pragma once
#include <random>
#include <vector>
#include <set>
#include <unordered_set>
#include <limits>
#include <cstddef> // std::size_t
#include <cstdint> // std::uint32_t, std::uint64_t
#include <algorithm>

namespace xcspp
{

    // Random utility
    class Random
    {
    private:
        // FIXME: do not use mutable
        mutable std::mt19937 m_engine;

    public:
        Random()
            : m_engine(std::random_device{}())
        {
        }

        explicit Random(std::uint32_t seed)
            : m_engine(seed)
        {
        }

        template <typename T = double>
        T nextDouble(T min = 0.0, T max = 1.0) const
        {
            return std::uniform_real_distribution<T>(min, max)(m_engine);
        }

        template <typename T = int>
        T nextInt(T min, T max) const
        {
            return std::uniform_int_distribution<T>(min, max)(m_engine);
        }

        template <typename T>
        const T & chooseFrom(const std::vector<T> & container) const
        {
            if (container.empty())
            {
                throw std::invalid_argument("Random::chooseFrom() received an empty container.");
            }

            return *(container.cbegin() + nextInt<std::size_t>(0, container.size() - 1));
        }

        template <typename T>
        T chooseFrom(const std::set<T> & container) const
        {
            // Create temporary std::vector from std::set and call the first chooseFrom function
            std::vector<T> vec(container.cbegin(), container.cend());
            return chooseFrom(vec);
        }

        template <typename T>
        T chooseFrom(const std::unordered_set<T> & container) const
        {
            // Create temporary std::vector from std::unordered_set and call the first chooseFrom function
            std::vector<T> vec(container.cbegin(), container.cend());
            return chooseFrom(vec);
        }

        template <typename T>
        std::size_t rouletteWheelSelection(const std::vector<T> & container) const
        {
            // Prepare a roulette wheel by the weights
            T sum = 0;
            std::vector<T> rouletteWheel;
            rouletteWheel.reserve(container.size());
            for (const auto & value : container)
            {
                sum += value;
                rouletteWheel.push_back(sum);
            }

            if (sum <= static_cast<T>(0))
            {
                throw std::runtime_error("Random::rouletteWheelSelection() generated an invalid weight sum.");
            }

            // Spin the roulette wheel
            const T randValue = nextDouble<T>(0, sum);
            const auto it = std::lower_bound(std::cbegin(rouletteWheel), std::cend(rouletteWheel), randValue);

            // Returns index of selected item
            return std::distance(std::cbegin(rouletteWheel), it);
        }

        template <typename T>
        std::size_t greedySelection(const std::vector<T> & container) const
        {
            T maxValue = *std::max_element(std::begin(container), std::end(container));
            std::vector<std::size_t> maxValueIdxs;

            if (container.empty())
            {
                throw std::invalid_argument("Random::greedySelection() received an empty container.");
            }

            for (std::size_t i = 0; i < container.size(); ++i)
            {
                if (container[i] == maxValue)
                {
                    maxValueIdxs.push_back(i);
                }
            }

            if (maxValueIdxs.empty())
            {
                throw std::runtime_error("Random::greedySelection() cannot select an element from the container.");
            }

            return chooseFrom(maxValueIdxs);
        }

        template <typename T>
        std::size_t epsilonGreedySelection(const std::vector<T> & container, double epsilon) const
        {
            std::size_t selectedIdx;

            if (nextDouble() < epsilon)
            {
                // Random selection
                if (container.empty())
                {
                    throw std::invalid_argument("Random::epsilonGreedySelection() received an empty container.");
                }
                selectedIdx = nextInt<std::size_t>(0, container.size() - 1);
            }
            else
            {
                // Greedy selection
                selectedIdx = greedySelection(container);
            }

            return selectedIdx;
        }

        template <typename T>
        std::size_t tournamentSelection(const std::vector<T> & container, double tau) const
        {
            std::size_t selectedIdx = container.size() - 1;
            T best = std::numeric_limits<T>::lowest();

            for (std::size_t i = 0; i < container.size(); ++i)
            {
                if (nextDouble() < tau && best < container[i])
                {
                    best = container[i];
                    selectedIdx = i;
                }
            }

            if (best == std::numeric_limits<T>::lowest())
            {
                return nextInt<std::size_t>(0, container.size() - 1);
            }
            else
            {
                return selectedIdx;
            }
        }

        template <typename T>
        std::size_t tournamentSelectionMicroClassifier(const std::vector<std::pair<T, std::uint64_t>> & container, double tau) const
        {
            std::size_t selectedIdx = container.size() - 1;
            T best = std::numeric_limits<T>::lowest();

            for (std::size_t i = 0; i < container.size(); ++i)
            {
                const auto & [ weight, numerosity ] = container[i];

                if (best < weight / numerosity)
                {
                    for (std::uint64_t j = 0; j < numerosity; ++j)
                    {
                        if (nextDouble() < tau)
                        {
                            best = weight / numerosity;
                            selectedIdx = i;
                            break;
                        }
                    }
                }
            }

            if (best == std::numeric_limits<T>::lowest())
            {
                return nextInt<std::size_t>(0, container.size() - 1);
            }
            else
            {
                return selectedIdx;
            }
        }
    };

}
