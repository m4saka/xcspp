#pragma once
#include <istream>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <cstddef>

#include "dataset.hpp"
#include "xcspp/xcs/classifier.hpp"

namespace xcspp
{

    namespace CSV
    {
        template <typename T>
        std::vector<std::vector<T>> LoadCSV(std::istream & is, bool rounds = false);

        template <typename T>
        std::vector<std::vector<T>> LoadCSVFile(const std::string & filename, bool rounds = false);

        template <typename T>
        void SaveCSV(std::ostream & os, const std::vector<std::vector<T>> & data);

        template <typename T>
        void SaveCSVFile(const std::string & filename, const std::vector<std::vector<T>> & data);

        Dataset ReadDataset(std::istream & is, bool rounds = false);

        Dataset ReadDatasetFromFile(const std::string & filename, bool rounds = false);

        std::vector<Classifier> ReadClassifiers(std::istream & is, bool skipFirstLine = true, bool skipFirstColumn = false);

        std::vector<Classifier> ReadClassifiersFromFile(const std::string & filename, bool skipFirstLine = true, bool skipFirstColumn = false);

        // --- Implementation of the function templates from here ---

        template <typename T>
        std::vector<std::vector<T>> LoadCSV(std::istream & is, bool rounds)
        {
            std::vector<std::vector<T>> data;

            // Read all lines in csv
            std::string line;
            while (std::getline(is, line) && !line.empty())
            {
                // Split comma-separated string
                std::istringstream iss(line);
                std::string field;
                std::vector<T> row;
                while (std::getline(iss, field, ','))
                {
                    if constexpr (std::is_arithmetic_v<T>)
                    {
                        // T = int, double, ...
                        double fieldValue = std::stof(field);
                        row.push_back(static_cast<T>(rounds ? std::round(fieldValue) : fieldValue));
                    }
                    else
                    {
                        // T = std::string, ...
                        row.push_back(field);
                    }
                }

                data.push_back(row);
            }

            return data;
        }

        template <typename T>
        std::vector<std::vector<T>> LoadCSVFile(const std::string & filename, bool rounds)
        {
            std::ifstream ifs(filename);
            if (!ifs.good())
            {
                throw std::runtime_error("CSV::LoadCSVFile: Failed to open the file '" + filename + "'.");
            }
            return LoadCSV<T>(ifs, rounds);
        }

        template <typename T>
        void SaveCSV(std::ostream & os, const std::vector<std::vector<T>> & data)
        {
            for (const auto & line : data)
            {
                for (std::size_t i = 0; i < line.size(); ++i)
                {
                    os << line[i];
                    if (i != line.size() - 1)
                    {
                        os << ',';
                    }
                }
                os << '\n';
            }
        }

        template <typename T>
        void SaveCSVFile(const std::string & filename, const std::vector<std::vector<T>> & data)
        {
            std::ofstream ofs(filename);
            if (!ofs.good())
            {
                throw std::runtime_error("CSV::SaveCSVFile: Failed to open the file '" + filename + "'.");
            }
            return SaveCSV(ofs, data);
        }
    }

}
