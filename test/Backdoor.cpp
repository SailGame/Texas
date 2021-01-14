#include "Backdoor.h"

void DummyBackdoor::SetDeck(const std::vector<texas_defines::card_t> &deck) {
  dm.deck.resize(0);
  dm.deck.insert(dm.deck.end(), deck.begin(), deck.end());
}

void DummyBackdoor::RedealCards() {
  for (auto &player : dm.players) {
    player.second.holecards.resize(0);
    dm.NextCard(player.first);
    dm.NextCard(player.first);
  }
}
