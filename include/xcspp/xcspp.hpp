#pragma once

#include "core/xcs/action_set.hpp"
#include "core/xcs/classifier.hpp"
#include "core/xcs/classifier_ptr_set.hpp"
#include "core/xcs/condition.hpp"
#include "core/xcs/ga.hpp"
#include "core/xcs/match_set.hpp"
#include "core/xcs/population.hpp"
#include "core/xcs/prediction_array.hpp"
#include "core/xcs/symbol.hpp"
#include "core/xcs/xcs.hpp"
#include "core/xcs/xcs_params.hpp"

namespace xcspp
{
    using xcs::XCS;
    using xcs::XCSParams;
}

#include "core/xcsr/action_set.hpp"
#include "core/xcsr/classifier.hpp"
#include "core/xcsr/classifier_ptr_set.hpp"
#include "core/xcsr/condition.hpp"
#include "core/xcsr/ga.hpp"
#include "core/xcsr/match_set.hpp"
#include "core/xcsr/population.hpp"
#include "core/xcsr/prediction_array.hpp"
#include "core/xcsr/symbol.hpp"
#include "core/xcsr/xcsr.hpp"
#include "core/xcsr/xcsr_params.hpp"
#include "core/xcsr/xcsr_repr.hpp"

namespace xcspp
{
    using xcsr::XCSR;
    using xcsr::XCSRParams;
}

#include "environment/ienvironment.hpp"
#include "environment/multiplexer_environment.hpp"
#include "environment/even_parity_environment.hpp"
#include "environment/majority_on_environment.hpp"
#include "environment/block_world_environment.hpp"
#include "environment/dataset_environment.hpp"

#include "helper/experiment_helper.hpp"
#include "helper/experiment_log_stream.hpp"
#include "helper/experiment_settings.hpp"
#include "helper/simple_moving_average.hpp"

#include "util/csv.hpp"
#include "util/dataset.hpp"
#include "util/random.hpp"
