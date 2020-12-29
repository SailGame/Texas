#include "TexasScore.h"
#include "Dummy.h"

#include <functional>
#include <map>
#include <vector>

#define CARD_COL(c) (c & 0xf0)
#define CARD_NUM(c) (c & 0xf)

using poker::cardarr_t;

namespace {
int royal_straight_flush(const cardarr_t &cards);
int straight_flush(const cardarr_t &cards);
int four_of_a_kind(const cardarr_t &cards);
int full_house(const cardarr_t &cards);
int flush(const cardarr_t &cards);
int straight(const cardarr_t &cards);
int three_of_a_kind(const cardarr_t &cards);
int two_pair(const cardarr_t &cards);
int one_pair(const cardarr_t &cards);
int high_card(const cardarr_t &cards);

int _compare(const int a, const int b);
int _for_each_combination(std::function<int(const cardarr_t)> &lambda,
                          const cardarr_t &cards);
} // namespace

const Dummy::score_t poker::_score(cardarr_t cards) {
  std::sort(cards.begin(), cards.end(), [](int a, int b) { return a > b; });
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
  return CardScore{royal_straight_flush(cards),
                   straight_flush(cards),
                   four_of_a_kind(cards),
                   full_house(cards),
                   flush(cards),
                   straight(cards),
                   three_of_a_kind(cards),
                   two_pair(cards),
                   one_pair(cards),
                   high_card(cards)};
}

const Dummy::score_t Dummy::Score(const uid_t uid) {
  // TODO: Record the winner cards into attribute?
  auto cards = holecards[uid];
  cards.insert(cards.end(), board.begin(), board.end());
  return poker::_score(cards);
}

int CardScore::Compare(const CardScore &rhs) const {
  int ret;
  if ((ret = _compare(royal_straight_flush, rhs.royal_straight_flush)))
    return ret;
  if ((ret = _compare(straight_flush, rhs.straight_flush)))
    return ret;
  if ((ret = _compare(four_of_a_kind, rhs.four_of_a_kind)))
    return ret;
  if ((ret = _compare(full_house, rhs.full_house)))
    return ret;
  if ((ret = _compare(flush, rhs.flush)))
    return ret;
  if ((ret = _compare(straight, rhs.straight)))
    return ret;
  if ((ret = _compare(three_of_a_kind, rhs.three_of_a_kind)))
    return ret;
  if ((ret = _compare(two_pair, rhs.two_pair)))
    return ret;
  if ((ret = _compare(one_pair, rhs.one_pair)))
    return ret;
  return _compare(high_card, rhs.high_card);
}

namespace {
using cardcnt_t = std::map<int, int>;

int royal_straight_flush(const cardarr_t &cards) {
  return straight_flush(cards) == 14;
}

int straight_flush(const cardarr_t &cards) {
  return flush(cards) ? straight(cards) : 0;
}

int four_of_a_kind(const cardarr_t &cards) {
  cardcnt_t cnt;
  for (const auto card : cards) {
    const auto num = CARD_NUM(card);
    if (cnt.count(num))
      ++cnt[num];
    else
      cnt[num] = 1;
  }
  for (const auto &ent : cnt)
    if (ent.second == 4)
      return ent.first;
  return 0;
}

int full_house(const cardarr_t &cards) {
  int three = three_of_a_kind(cards), two = two_pair(cards);
  if (three && two) {
    int ret = three << 4;
    if ((two & 0xf) == three)
      ret |= (two & 0xf0) >> 4;
    else
      ret |= (two & 0xf);
    return ret;
  }
  return 0;
}

int flush(const cardarr_t &cards) {
  int color = cards[0] & 0xf0;
  for (auto card : cards)
    if ((card & 0xf0) != color)
      return 0;
  return high_card(cards);
}

int straight(const cardarr_t &cards) {
  // A K Q 10 9 => 14
  // 5 4 3 2 1  => 5
  // Return the biggest num if being a straight.
  const int head = CARD_NUM(cards[0]);
  for (int i = 1; i < 5; ++i)
    if (CARD_NUM(cards[i]) != head - i) {
      if (i == 4 && head == 13 && CARD_NUM(cards[4]) == 1)
        return 14;
      else
        return 0;
    }
  return head;
}

int three_of_a_kind(const cardarr_t &cards) {
  cardcnt_t cnt;
  for (const auto card : cards) {
    const auto num = CARD_NUM(card);
    if (cnt.count(num))
      ++cnt[num];
    else
      cnt[num] = 1;
  }
  for (const auto &ent : cnt)
    if (ent.second == 3)
      return ent.first;
  return 0;
}

int two_pair(const cardarr_t &cards) {
  cardcnt_t cnt;
  for (const auto card : cards) {
    const auto num = CARD_NUM(card);
    if (cnt.count(num))
      ++cnt[num];
    else
      cnt[num] = 1;
  }
  int a = 0, b = 0;
  for (const auto &ent : cnt)
    if (ent.second == 2) {
      if (a)
        b = ent.first;
      else
        a = ent.first;
    }
  if (a && b) {
    if (a < b)
      std::swap(a, b);
    b |= (a << 4);
    return b;
  }
  return 0;
}

int one_pair(const cardarr_t &cards) {
  cardcnt_t cnt;
  for (const auto card : cards) {
    const auto num = CARD_NUM(card);
    if (cnt.count(num))
      ++cnt[num];
    else
      cnt[num] = 1;
  }
  for (const auto &ent : cnt)
    if (ent.second == 2)
      return ent.first;
  return 0;
}

int high_card(const cardarr_t &cards) {
  int ret = 0;
  for (const auto card : cards)
    ret |= 1 << CARD_NUM(card);
  return ret;
}

int _compare(const int a, const int b) {
  if (a > b)
    return 1;
  if (a == b)
    return 0;
  return -1;
}

int _for_each_combination(std::function<int(const cardarr_t)> &lambda,
                          const cardarr_t &cards) {
  const auto len = cards.size();
  assert(len == 7);

  int ret = 0, idx = 0;
  std::vector<cardarr_t> combinations(21, cardarr_t());
  for (int i = 0; i < 3; ++i)
    for (int j = i + 1; j < 4; ++j)
      for (int k = j + 1; k < 5; ++k)
        for (int l = k + 1; l < 6; ++l)
          for (int m = l + 1; m < 7; ++m) {
            combinations[idx].push_back(cards[i]);
            combinations[idx].push_back(cards[j]);
            combinations[idx].push_back(cards[k]);
            combinations[idx].push_back(cards[l]);
            combinations[idx].push_back(cards[m]);
            ++idx;
          }
  for (const auto &com : combinations)
    ret = std::max(ret, lambda(com));

  return ret;
}
} // namespace
