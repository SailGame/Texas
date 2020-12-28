#include "Dummy.h"

namespace {
  using cardarr_t = std::vector<Dummy::card_t>;

  // bool royal_straight_flush(const cardarr_t &cards);
  // bool straight_flush(const cardarr_t &cards);
  // bool four_of_a_kind(const cardarr_t &cards);
  // bool full_house(const cardarr_t &cards);
  // bool flush(const cardarr_t &cards);
  // bool straight(const cardarr_t &cards);
  // bool three_of_a_kind(const cardarr_t &cards);
  // bool two_pair(const cardarr_t &cards);
  // bool one_pair(const cardarr_t &cards);
  int high_card(const cardarr_t &cards);
}

const int Dummy::Score(const uid_t uid) {
  // TODO: Record the winner cards into attribute?

  // TODO: Evalute the score for @uid.
  const auto &cards = holecards[uid];

  // 1. Royal Straight Flush

  // 2. Straight Flush
  // 3. Four of a kind
  // 4. Full house
  // 5. Flush
  // 6. Straight
  // 7. Three of a kind
  // 8. Two pair
  // 9. One Pair
  // 10. High card

  return 0;
}

namespace {
  // bool royal_straight_flush(const cardarr_t &cards) {}
  // bool straight_flush(const cardarr_t &cards) {}
  // bool four_of_a_kind(const cardarr_t &cards) {}
  // bool full_house(const cardarr_t &cards) {}
  // bool flush(const cardarr_t &cards) {}
  // bool straight(const cardarr_t &cards) {}
  // bool three_of_a_kind(const cardarr_t &cards) {}
  // bool two_pair(const cardarr_t &cards) {}
  // bool one_pair(const cardarr_t &cards) {}
  int high_card(const cardarr_t &cards) {
    int ret = 0;
    for(const auto card : cards)
      ret |= 1 << (card & 0xf);
    return ret;
  }
}
