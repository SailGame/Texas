#ifndef SAILGAME_TEXAS_DEFINES
#define SAILGAME_TEXAS_DEFINES

#include <vector>

namespace texas_defines {

struct CardScore;

using uid_t = int;
using chip_t = int;
using card_t = int;
using status_t = int;
using score_t = CardScore;

struct PlayerStat {
  const std::string addr;
  std::vector<card_t> holecards;
  chip_t bankroll, roundbets;
  int alive, allin;

  explicit PlayerStat(const std::string &addr) : addr(addr) {}
};

struct GameStatus {
  std::vector<card_t> board, personal;
  status_t state;
  explicit GameStatus(status_t state) : state(state) {}
};

struct CardScore {
  int royal_straight_flush;
  int straight_flush;
  int four_of_a_kind;
  int full_house;
  int flush;
  int straight;
  int three_of_a_kind;
  int two_pair;
  int one_pair;
  int high_card;

  int Compare(const CardScore &rhs) const;
};

} // namespace texas_defines

#endif
