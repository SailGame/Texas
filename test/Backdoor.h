#ifndef SAILGAME_TEXAS_BACKDOOR
#define SAILGAME_TEXAS_BACKDOOR

#include "../src/Dummy.h"
#include <iosfwd>

class DummyBackdoor {
public:
  void DumpDebugMessages(const Dummy &dm, std::ostream &out);
  void SetDeck(Dummy &dm, const std::vector<Dummy::card_t> &deck);
  void RedealCards(Dummy &dm);
  int CountPlayers(const Dummy &dm) { return dm.user_count; }
};

#endif
