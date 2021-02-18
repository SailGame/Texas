#pragma once

#include <tcore/Dummy.h>
#include <tcore/defines.h>

namespace SailGame {
namespace Texas {
class DummyBackdoor {
  Dummy &dm;

public:
  explicit DummyBackdoor(Dummy &dm) : dm(dm) {}

  // Setters
  void SetDeck(const std::vector<texas_defines::card_t> &deck);
  void RedealCards();
  void Liquidate() { dm.Liquidate(); }

  // Getters
  int CountPlayers() const { return dm.user_count; }
  auto &GetPlayerStat() { return dm.players; }
  std::vector<texas_defines::uid_t> &GetWinners() { return dm.winners; }
  const std::vector<texas_defines::card_t> &GetBoard() const {
    return dm.board;
  }
  const texas_defines::chip_t GetCurrentChips() { return dm.cur_chips; }
};

} // namespace Texas
} // namespace SailGame