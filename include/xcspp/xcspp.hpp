#pragma once

#include "core/xcs/action_set.hpp"
#include "core/xcs/classifier.hpp"
#include "core/xcs/classifier_ptr_set.hpp"
#include "core/xcs/condition.hpp"
#include "core/xcs/xcs_params.hpp"
#include "core/xcs/ga.hpp"
#include "core/xcs/match_set.hpp"
#include "core/xcs/population.hpp"
#include "core/xcs/prediction_array.hpp"
#include "core/xcs/symbol.hpp"
#include "core/xcs/xcs.hpp"

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
