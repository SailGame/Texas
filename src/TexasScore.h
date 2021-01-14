#ifndef SAILGAME_TEXAS_SCORE
#define SAILGAME_TEXAS_SCORE

#include "Dummy.h"
#include "defines.h"

namespace poker {
using cardarr_t = std::vector<texas_defines::card_t>;
const texas_defines::score_t _score(cardarr_t cards);
} // namespace poker

#endif
