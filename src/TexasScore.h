#pragma once

#include "Dummy.h"
#include "defines.h"

namespace SailGame {
namespace Texas {
namespace poker {

using cardarr_t = std::vector<texas_defines::card_t>;
const texas_defines::score_t _score(cardarr_t cards);

} // namespace poker
} // namespace Texas
} // namespace SailGame