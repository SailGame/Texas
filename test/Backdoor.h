#ifndef SAILGAME_TEXAS_BACKDOOR
#define SAILGAME_TEXAS_BACKDOOR

#include <iosfwd>
#include <src/Dummy.h>
#include <src/defines.h>

class DummyBackdoor {
  Dummy &dm;

public:
  explicit DummyBackdoor(Dummy &dm) : dm(dm) {}

  // void DumpDebugMessages(std::ostream &out) const;

  void SetDeck(const std::vector<texas_defines::card_t> &deck);
  void RedealCards();
  int CountPlayers() const { return dm.user_count; }
  auto &GetPlayerStat() { return dm.players; }
  const texas_defines::uid_t GetPrevWinner() const { return dm.prev_winner; }
  const std::vector<texas_defines::card_t> &GetBoard() const {
    return dm.board;
  }
  const texas_defines::chip_t GetCurrentChips() { return dm.cur_chips; }
};

#endif
