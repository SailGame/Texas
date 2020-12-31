#ifndef SAILGAME_TEXAS_SCORE
#define SAILGAME_TEXAS_SCORE

#include "Dummy.h"

namespace poker {
using cardarr_t = std::vector<Dummy::card_t>;
const Dummy::score_t _score(cardarr_t cards);
} // namespace poker

#endif
