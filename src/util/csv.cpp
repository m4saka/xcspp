#include "xcspp/util/csv.hpp"

#include "xcspp/util/dataset.hpp"
#include "xcspp/core/xcs/classifier.hpp"

namespace xcspp
{

    namespace CSV
    {
        Dataset ReadDataset(std::istream & is, bool rounds)
        {
            std::vector<std::vector<int>> situations;
            std::vector<int> actions;

            // Read all lines in csv
            std::string line;
            while (std::getline(is, line) && !line.empty())
            {
                // Split comma-separated string
                std::istringstream iss(line);
                std::string field;
                double fieldValue = 0.0;
                std::vector<int> situation;
                while (std::getline(iss, field, ','))
                {
                    fieldValue = std::stof(field);
                    situation.push_back(static_cast<int>(rounds ? std::round(fieldValue) : fieldValue));
                }

                // Skip line with no values
                if (situation.empty())
                {
                    continue;
                }

                // Last field is action
                actions.push_back(static_cast<int>(fieldValue));
                situation.pop_back();

                situations.push_back(situation);
            }

            return { situations, actions };
        }

        Dataset ReadDatasetFromFile(const std::string & filename, bool rounds)
        {
            std::ifstream ifs(filename);
            if (!ifs.good())
            {
                throw std::runtime_error("CSV::ReadDatasetFromFile: Failed to open the file '" + filename + "'.");
            }
            return ReadDataset(ifs, rounds);
        }

    }

}
