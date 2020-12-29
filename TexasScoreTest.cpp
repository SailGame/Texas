#include "TexasScore.h"
#include "Dummy.h"

#include <iostream>

void is_equal(int, int, int);
inline void assert_equal(int a, int b) { is_equal(a, b, __LINE__); }

int main() {
  using namespace poker;

  // Test evalutaion for high card.
  {
    // Test generation correctness 1.
    const cardarr_t cards{H1, H2, H3, H4, C6};
    const auto ret = _score(cards);
    assert_equal(ret.royal_straight_flush, 0);
    assert_equal(ret.straight_flush, 0);
    assert_equal(ret.four_of_a_kind, 0);
    assert_equal(ret.full_house, 0);
    assert_equal(ret.flush, 0);
    assert_equal(ret.straight, 0);
    assert_equal(ret.three_of_a_kind, 0);
    assert_equal(ret.two_pair, 0);
    assert_equal(ret.one_pair, 0);
    assert_equal(ret.high_card, 94); // 1011110
  }
  {
    // Test generation correctness 2.
    const cardarr_t cards{H1, H2, D5, S10, CK};
    const auto ret = _score(cards);
    assert_equal(ret.royal_straight_flush, 0);
    assert_equal(ret.straight_flush, 0);
    assert_equal(ret.four_of_a_kind, 0);
    assert_equal(ret.full_house, 0);
    assert_equal(ret.flush, 0);
    assert_equal(ret.straight, 0);
    assert_equal(ret.three_of_a_kind, 0);
    assert_equal(ret.two_pair, 0);
    assert_equal(ret.one_pair, 0);
    assert_equal(ret.high_card, 9254); // 10010000100110
  }
  {
    // Test comparision correctness 1.
    const cardarr_t cards1{H1, H2, D5, S10, CK};
    const cardarr_t cards2{H1, H2, H3, H4, C6};
    assert_equal(_score(cards1).Compare(_score(cards2)), 1);
    assert_equal(_score(cards1).Compare(_score(cards1)), 0);
  }

  return 0;
}

void is_equal(int src, int target, int linum) {
  if (src == target)
    return;
  std::cerr << "At line: " << linum << ", expect [" << target << "], but got ["
            << src << ']' << std::endl;
  exit(1);
}
