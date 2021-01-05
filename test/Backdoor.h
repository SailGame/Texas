#ifndef SAILGAME_TEXAS_BACKDOOR
#define SAILGAME_TEXAS_BACKDOOR

#include <iosfwd>
#include <src/Dummy.h>
#include <src/defines.h>

class DummyBackdoor {
  Dummy &dm;

public:
  explicit DummyBackdoor(Dummy &dm) : dm(dm) {}

  void DumpDebugMessages(std::ostream &out) const;

  void SetDeck(const std::vector<texas_defines::card_t> &deck);
  void RedealCards();
  const std::map<texas_defines::uid_t, std::vector<texas_defines::card_t>> &
  GetHolecards() const {
    return dm.holecards;
  }
  const std::vector<texas_defines::card_t> &GetBoard() const {
    return dm.board;
  }
  const std::map<texas_defines::uid_t, texas_defines::chip_t> &
  GetRoundbets() const {
    return dm.roundbets;
  }
  int CountPlayers() { return dm.user_count; }
  const texas_defines::chip_t GetCurrentChips() { return dm.cur_chips; }
};

#endif
