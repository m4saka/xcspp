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
        Dataset readDataset(std::istream & is, bool rounds = false);

        Dataset readDataset(const std::string & filename, bool rounds = false);

        std::vector<Classifier> readClassifiers(std::istream & is, bool skipFirstLine = true, bool skipFirstColumn = false);

        std::vector<Classifier> readClassifiers(const std::string & filename, bool skipFirstLine = true, bool skipFirstColumn = false);
    }

}
