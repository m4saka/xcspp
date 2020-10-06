#include "xcspp/util/csv.hpp"

#include "xcspp/util/dataset.hpp"
#include "xcspp/classifier.hpp"

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

        std::vector<Classifier> ReadClassifiers(std::istream & is, bool skipFirstLine, bool skipFirstColumn)
        {
            std::vector<Classifier> classifiers;

            std::string line;

            if (skipFirstLine)
            {
                std::getline(is, line);
            }

            // Load all lines from csv
            while (std::getline(is, line) && !line.empty())
            {
                // Split comma-separated string
                std::istringstream iss(line);
                std::string field;
                std::vector<int> situation;

                // For XCSR population CSV, we need an option whether to skip the first graphic notation column
                if (skipFirstColumn)
                {
                    std::getline(iss, field, ',');
                }

                Condition condition;
                int action = 0;
                double prediction = 0.0;
                double epsilon = 0.0;
                double fitness = 0.0;
                std::size_t experience = 0;
                std::size_t timeStamp = 0;
                double actionSetSize = 0.0;
                std::size_t numerosity = 1;

                int columnIdx = 0;
                while (std::getline(iss, field, ','))
                {
                    if (columnIdx == 0) // Condition
                    {
                        condition = Condition(field);
                    }
                    else
                    {
                        std::istringstream fieldIss(field);
                        switch (columnIdx)
                        {
                        case 1: // Action
                            fieldIss >> action;
                            break;
                        case 2: // Prediction
                            fieldIss >> prediction;
                            break;
                        case 3: // Epsilon
                            fieldIss >> epsilon;
                            break;
                        case 4: // Fitness
                            fieldIss >> fitness;
                            break;
                        case 5: // Experience
                            fieldIss >> experience;
                            break;
                        case 6: // TimeStamp
                            fieldIss >> timeStamp;
                            break;
                        case 7: // ActionSetSize
                            fieldIss >> actionSetSize;
                            break;
                        case 8: // Numerosity
                            fieldIss >> numerosity;
                            break;
                        }
                    }
                    ++columnIdx;
                }

                // Add classifier to list
                Classifier cl(condition, action, prediction, epsilon, fitness, timeStamp);
                cl.experience = experience;
                cl.actionSetSize = actionSetSize;
                cl.numerosity = numerosity;
                classifiers.push_back(std::move(cl));
            }

            return classifiers;
        }

        std::vector<Classifier> ReadClassifiersFromFile(const std::string & filename, bool skipFirstLine, bool skipFirstColumn)
        {
            std::ifstream ifs(filename);
            if (!ifs.good())
            {
                throw std::runtime_error("CSV::ReadClassifiersFromFile: Failed to open the file '" + filename + "'.");
            }
            return ReadClassifiers(ifs, skipFirstLine, skipFirstColumn);
        }
    }

}
