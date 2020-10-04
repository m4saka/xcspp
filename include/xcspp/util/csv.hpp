#pragma once
#include <istream>
#include <sstream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstddef>

#include "dataset.hpp"
#include "xcspp/classifier.hpp"

namespace xcspp
{

    namespace CSV
    {
        Dataset ReadDataset(std::istream & is, bool rounds = false);

        Dataset ReadDataset(const std::string & filename, bool rounds = false);

        std::vector<Classifier> ReadClassifiers(std::istream & is, bool skipFirstLine = true, bool skipFirstColumn = false);

        std::vector<Classifier> ReadClassifiers(const std::string & filename, bool skipFirstLine = true, bool skipFirstColumn = false);
    }

}
