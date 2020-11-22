#pragma once

#include "experiment/xcs/action_set.hpp"
#include "experiment/xcs/classifier.hpp"
#include "experiment/xcs/classifier_ptr_set.hpp"
#include "experiment/xcs/condition.hpp"
#include "experiment/xcs/xcs_params.hpp"
#include "experiment/xcs/ga.hpp"
#include "experiment/xcs/match_set.hpp"
#include "experiment/xcs/population.hpp"
#include "experiment/xcs/prediction_array.hpp"
#include "experiment/xcs/symbol.hpp"
#include "experiment/xcs/xcs.hpp"

namespace xcspp
{
    // xcspp::XCS = xcspp::xcs::XCS
    using xcs::XCS;

    // xcs::XCSParams = xcspp::XCSParams
    using xcs::XCSParams;
}

#include "environment/ienvironment.hpp"
#include "environment/multiplexer_environment.hpp"
#include "environment/block_world_environment.hpp"
#include "environment/dataset_environment.hpp"

#include "helper/experiment_helper.hpp"
#include "helper/experiment_log_stream.hpp"
#include "helper/experiment_settings.hpp"
#include "helper/simple_moving_average.hpp"

#include "util/csv.hpp"
#include "util/dataset.hpp"
#include "util/random.hpp"
