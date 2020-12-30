#include "Backdoor.h"

#include <iostream>

void DummyBackdoor::DumpDebugMessages(const Dummy &dm, std::ostream &out) {
  using namespace std;
  const string divider(30, '=');
  out << divider << endl;
  out << "State: " << static_cast<int>(dm.state)
      << "\t\tThe number of players: " << dm.user_count
      << "\t\tBoard: " << std::hex;
  for (auto const card : dm.board)
    out << card << ' ';
  out << std::dec << '[' << dm.board.size() << ']' << endl;
  out << "Deck: " << std::hex;
  for (auto const card : dm.deck)
    out << card << ' ';
  out << std::dec << '[' << dm.deck.size() << ']' << endl;
  out << "Previous pos: " << dm.prev_pos << "\t\tButton pos: " << dm.button
      << "\t\tCutoff: " << dm.small_blind << "\t\tChips: " << dm.cur_chips
      << endl;
  out << "Someone raised this turn? " << std::boolalpha << dm.raised << std::dec
      << endl;
  for (uid_t uid = 1; uid <= dm.LastPlayer(); ++uid) {
    out << endl;
    out << "\t[" << dm.uid2addr.at(uid) << "]\tuid: " << uid << endl;
    out << "\tholecards: " << std::hex;
    for (auto const card : dm.holecards.at(uid))
      out << card << ' ';
    out << std::dec << endl;
    out << "\tbankroll: " << dm.bankroll.at(uid) << endl;
    out << "\troundbets: " << dm.roundbets.at(uid) << endl;
    out << "\talive?: " << dm.alive.at(uid)
        << "\t\tall-in?: " << dm.allin.at(uid) << endl;
  }
  out << divider << endl;
}

void DummyBackdoor::SetDeck(Dummy &dm, const std::vector<Dummy::card_t> &deck) {
  dm.deck.resize(0);
  dm.deck.insert(dm.deck.end(), deck.begin(), deck.end());
}

void DummyBackdoor::RedealCards(Dummy &dm) {
  for (auto &player : dm.holecards) {
    player.second.resize(0);
    dm.NextCard(player.first);
    dm.NextCard(player.first);
  }
}
