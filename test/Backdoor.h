#ifndef SAILGAME_TEXAS_BACKDOOR
#define SAILGAME_TEXAS_BACKDOOR

#include <iosfwd>
#include <src/Dummy.h>

class DummyBackdoor {
  Dummy &dm;

public:
  explicit DummyBackdoor(Dummy &dm) : dm(dm) {}

  void DumpDebugMessages(std::ostream &out) const;

  void SetDeck(const std::vector<Dummy::card_t> &deck);
  void RedealCards();
  int CountPlayers() { return dm.user_count; }
};

#endif
