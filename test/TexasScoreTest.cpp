#include <gtest/gtest.h>
#include <src/Dummy.h>
#include <src/TexasScore.h>

using namespace poker;

TEST(ScoreTest, ScoreEvaluation) {

  {
    // High-card
    const cardarr_t cards{H1, H2, H3, H4, C6};
    const auto ret = _score(cards);
    EXPECT_EQ(ret.royal_straight_flush, 0);
    EXPECT_EQ(ret.straight_flush, 0);
    EXPECT_EQ(ret.four_of_a_kind, 0);
    EXPECT_EQ(ret.full_house, 0);
    EXPECT_EQ(ret.flush, 0);
    EXPECT_EQ(ret.straight, 0);
    EXPECT_EQ(ret.three_of_a_kind, 0);
    EXPECT_EQ(ret.two_pair, 0);
    EXPECT_EQ(ret.one_pair, 0);
    EXPECT_EQ(ret.high_card, 94); // 1011110
  }
  {
    // One-pair
    const cardarr_t cards{H1, H2, D2, D4, C5};
    const auto ret = _score(cards);
    EXPECT_EQ(ret.royal_straight_flush, 0);
    EXPECT_EQ(ret.straight_flush, 0);
    EXPECT_EQ(ret.four_of_a_kind, 0);
    EXPECT_EQ(ret.full_house, 0);
    EXPECT_EQ(ret.flush, 0);
    EXPECT_EQ(ret.straight, 0);
    EXPECT_EQ(ret.three_of_a_kind, 0);
    EXPECT_EQ(ret.two_pair, 0);
    EXPECT_EQ(ret.one_pair, 2);
    EXPECT_EQ(ret.high_card, 54); // 110110
  }
  {
    // two-pair
    const cardarr_t cards{H1, H2, D2, D4, C4};
    const auto ret = _score(cards);
    EXPECT_EQ(ret.royal_straight_flush, 0);
    EXPECT_EQ(ret.straight_flush, 0);
    EXPECT_EQ(ret.four_of_a_kind, 0);
    EXPECT_EQ(ret.full_house, 0);
    EXPECT_EQ(ret.flush, 0);
    EXPECT_EQ(ret.straight, 0);
    EXPECT_EQ(ret.three_of_a_kind, 0);
    EXPECT_EQ(ret.two_pair, 0x42);
    EXPECT_EQ(ret.one_pair, 4);
    EXPECT_EQ(ret.high_card, 22); // 10110
  }
  {
    // three of a kind
    const cardarr_t cards{H1, H2, D2, C2, C4};
    const auto ret = _score(cards);
    EXPECT_EQ(ret.royal_straight_flush, 0);
    EXPECT_EQ(ret.straight_flush, 0);
    EXPECT_EQ(ret.four_of_a_kind, 0);
    EXPECT_EQ(ret.full_house, 0);
    EXPECT_EQ(ret.flush, 0);
    EXPECT_EQ(ret.straight, 0);
    EXPECT_EQ(ret.three_of_a_kind, 2);
    EXPECT_EQ(ret.two_pair, 0);
    EXPECT_EQ(ret.one_pair, 0);
    EXPECT_EQ(ret.high_card, 22); // 10110
  }
  {
    // straight (1-2-3-4-5)
    const cardarr_t cards{H1, H2, D3, C4, C5};
    const auto ret = _score(cards);
    EXPECT_EQ(ret.royal_straight_flush, 0);
    EXPECT_EQ(ret.straight_flush, 0);
    EXPECT_EQ(ret.four_of_a_kind, 0);
    EXPECT_EQ(ret.full_house, 0);
    EXPECT_EQ(ret.flush, 0);
    EXPECT_EQ(ret.straight, 5);
    EXPECT_EQ(ret.three_of_a_kind, 0);
    EXPECT_EQ(ret.two_pair, 0);
    EXPECT_EQ(ret.one_pair, 0);
    EXPECT_EQ(ret.high_card, 62); // 111110
  }
  {
    // straight (10-J-Q-K-A)
    const cardarr_t cards{H1, H10, DJ, CQ, CK};
    const auto ret = _score(cards);
    EXPECT_EQ(ret.royal_straight_flush, 0);
    EXPECT_EQ(ret.straight_flush, 0);
    EXPECT_EQ(ret.four_of_a_kind, 0);
    EXPECT_EQ(ret.full_house, 0);
    EXPECT_EQ(ret.flush, 0);
    EXPECT_EQ(ret.straight, 14);
    EXPECT_EQ(ret.three_of_a_kind, 0);
    EXPECT_EQ(ret.two_pair, 0);
    EXPECT_EQ(ret.one_pair, 0);
    EXPECT_EQ(ret.high_card, 15362); // 11110000000010
  }
  {
    // flush
    const cardarr_t cards{H1, H2, H3, H4, H6};
    const auto ret = _score(cards);
    EXPECT_EQ(ret.royal_straight_flush, 0);
    EXPECT_EQ(ret.straight_flush, 0);
    EXPECT_EQ(ret.four_of_a_kind, 0);
    EXPECT_EQ(ret.full_house, 0);
    EXPECT_EQ(ret.flush, 94); // 1011110
    EXPECT_EQ(ret.straight, 0);
    EXPECT_EQ(ret.three_of_a_kind, 0);
    EXPECT_EQ(ret.two_pair, 0);
    EXPECT_EQ(ret.one_pair, 0);
    EXPECT_EQ(ret.high_card, 94); // same as flush
  }
  {
    // full-house
    const cardarr_t cards{H1, C1, D1, H2, C2};
    const auto ret = _score(cards);
    EXPECT_EQ(ret.royal_straight_flush, 0);
    EXPECT_EQ(ret.straight_flush, 0);
    EXPECT_EQ(ret.four_of_a_kind, 0);
    EXPECT_EQ(ret.full_house, 0x12);
    EXPECT_EQ(ret.flush, 0);
    EXPECT_EQ(ret.straight, 0);
    EXPECT_EQ(ret.three_of_a_kind, 1);
    EXPECT_EQ(ret.two_pair, 0);
    EXPECT_EQ(ret.one_pair, 2);
    EXPECT_EQ(ret.high_card, 6); // 110
  }
  {
    // four of a kind
    const cardarr_t cards{H1, C1, D1, S1, C2};
    const auto ret = _score(cards);
    EXPECT_EQ(ret.royal_straight_flush, 0);
    EXPECT_EQ(ret.straight_flush, 0);
    EXPECT_EQ(ret.four_of_a_kind, 1);
    EXPECT_EQ(ret.full_house, 0);
    EXPECT_EQ(ret.flush, 0);
    EXPECT_EQ(ret.straight, 0);
    EXPECT_EQ(ret.three_of_a_kind, 0);
    EXPECT_EQ(ret.two_pair, 0);
    EXPECT_EQ(ret.one_pair, 0);
    EXPECT_EQ(ret.high_card, 6); // 110
  }
  {
    // straight-flush
    const cardarr_t cards{H1, H2, H3, H4, H5};
    const auto ret = _score(cards);
    EXPECT_EQ(ret.royal_straight_flush, 0);
    EXPECT_EQ(ret.straight_flush, 5);
    EXPECT_EQ(ret.four_of_a_kind, 0);
    EXPECT_EQ(ret.full_house, 0);
    EXPECT_EQ(ret.flush, 62); // 111110
    EXPECT_EQ(ret.straight, 5);
    EXPECT_EQ(ret.three_of_a_kind, 0);
    EXPECT_EQ(ret.two_pair, 0);
    EXPECT_EQ(ret.one_pair, 0);
    EXPECT_EQ(ret.high_card, 62); // same as flush
  }
  {
    // royal-straight-flush
    const cardarr_t cards{H1, H10, HJ, HQ, HK};
    const auto ret = _score(cards);
    EXPECT_EQ(ret.royal_straight_flush, 1);
    EXPECT_EQ(ret.straight_flush, 14);
    EXPECT_EQ(ret.four_of_a_kind, 0);
    EXPECT_EQ(ret.full_house, 0);
    EXPECT_EQ(ret.flush, 15362); // 11110000000010
    EXPECT_EQ(ret.straight, 14);
    EXPECT_EQ(ret.three_of_a_kind, 0);
    EXPECT_EQ(ret.two_pair, 0);
    EXPECT_EQ(ret.one_pair, 0);
    EXPECT_EQ(ret.high_card, 15362); // same as flush
  }
}

TEST(ScoreTest, ScoreComparision) {

  {
    // Rank
    const cardarr_t cards_rsf{H1, H10, HJ, HQ, HK},
        cards_sf{H1, H2, H3, H4, H5}, cards_fok{H1, C1, D2, S2, H2},
        cards_fh{H1, C1, D1, H2, C2}, cards_f{H1, H2, H3, H4, H6},
        cards_s{H1, H2, H3, H4, D5}, cards_tok{H1, C1, D1, H2, C3},
        cards_tp{H1, C1, D2, H2, H3}, cards_op{H1, C1, D2, D3, D4},
        cards_hc{H1, H2, H3, H4, D6};
    const auto s_rsf = _score(cards_rsf), s_sf = _score(cards_sf),
               s_fok = _score(cards_fok), s_fh = _score(cards_fh),
               s_f = _score(cards_f), s_s = _score(cards_s),
               s_tok = _score(cards_tok), s_tp = _score(cards_tp),
               s_op = _score(cards_op), s_hc = _score(cards_hc);

    EXPECT_EQ(s_rsf.Compare(s_sf), 1);
    EXPECT_EQ(s_rsf.Compare(s_fok), 1);
    EXPECT_EQ(s_rsf.Compare(s_fh), 1);
    EXPECT_EQ(s_rsf.Compare(s_f), 1);
    EXPECT_EQ(s_rsf.Compare(s_s), 1);
    EXPECT_EQ(s_rsf.Compare(s_tok), 1);
    EXPECT_EQ(s_rsf.Compare(s_tp), 1);
    EXPECT_EQ(s_rsf.Compare(s_op), 1);
    EXPECT_EQ(s_rsf.Compare(s_hc), 1);

    EXPECT_EQ(s_sf.Compare(s_fok), 1);
    EXPECT_EQ(s_sf.Compare(s_fh), 1);
    EXPECT_EQ(s_sf.Compare(s_f), 1);
    EXPECT_EQ(s_sf.Compare(s_s), 1);
    EXPECT_EQ(s_sf.Compare(s_tok), 1);
    EXPECT_EQ(s_sf.Compare(s_tp), 1);
    EXPECT_EQ(s_sf.Compare(s_op), 1);
    EXPECT_EQ(s_sf.Compare(s_hc), 1);

    EXPECT_EQ(s_fok.Compare(s_fh), 1);
    EXPECT_EQ(s_fok.Compare(s_f), 1);
    EXPECT_EQ(s_fok.Compare(s_s), 1);
    EXPECT_EQ(s_fok.Compare(s_tok), 1);
    EXPECT_EQ(s_fok.Compare(s_tp), 1);
    EXPECT_EQ(s_fok.Compare(s_op), 1);
    EXPECT_EQ(s_fok.Compare(s_hc), 1);

    EXPECT_EQ(s_fh.Compare(s_f), 1);
    EXPECT_EQ(s_fh.Compare(s_s), 1);
    EXPECT_EQ(s_fh.Compare(s_tok), 1);
    EXPECT_EQ(s_fh.Compare(s_tp), 1);
    EXPECT_EQ(s_fh.Compare(s_op), 1);
    EXPECT_EQ(s_fh.Compare(s_hc), 1);

    EXPECT_EQ(s_f.Compare(s_s), 1);
    EXPECT_EQ(s_f.Compare(s_tok), 1);
    EXPECT_EQ(s_f.Compare(s_tp), 1);
    EXPECT_EQ(s_f.Compare(s_op), 1);
    EXPECT_EQ(s_f.Compare(s_hc), 1);

    EXPECT_EQ(s_s.Compare(s_tok), 1);
    EXPECT_EQ(s_s.Compare(s_tp), 1);
    EXPECT_EQ(s_s.Compare(s_op), 1);
    EXPECT_EQ(s_s.Compare(s_hc), 1);

    EXPECT_EQ(s_tok.Compare(s_tp), 1);
    EXPECT_EQ(s_tok.Compare(s_op), 1);
    EXPECT_EQ(s_tok.Compare(s_hc), 1);

    EXPECT_EQ(s_tp.Compare(s_op), 1);
    EXPECT_EQ(s_tp.Compare(s_hc), 1);

    EXPECT_EQ(s_op.Compare(s_hc), 1);
  }
  {
    // Uneven high-card
    const cardarr_t cards1{H1, H2, D5, S10, CK};
    const cardarr_t cards2{H1, H2, H3, H4, C6};
    EXPECT_EQ(_score(cards1).Compare(_score(cards2)), 1);
    EXPECT_EQ(_score(cards2).Compare(_score(cards1)), -1);
  }
  {
    // Even high-card
    const cardarr_t cards{H1, H2, D5, S10, CK};
    EXPECT_EQ(_score(cards).Compare(_score(cards)), 0);
  }
  {
    // Uneven one-pair
    const cardarr_t cards1{H1, H2, C2, H3, H4}, cards2{D1, D2, D4, C4, D5};
    EXPECT_EQ(_score(cards1).Compare(_score(cards2)), -1);
  }
  {
    // Even one-pair => uneven high-card
    const cardarr_t cards1{H1, H2, C2, H3, H4}, cards2{D1, D2, S2, D3, D5};
    EXPECT_EQ(_score(cards1).Compare(_score(cards2)), -1);
  }
  {
    // Even one-pair => even high-card
    const cardarr_t cards1{H1, H2, C2, H3, H4}, cards2{D1, D2, S2, D3, D4};
    EXPECT_EQ(_score(cards1).Compare(_score(cards2)), 0);
  }
  {
    // Uneven two-pair
    const cardarr_t cards1{H1, C1, C3, H3, H4}, cards2{H2, D2, S3, D3, D4};
    EXPECT_EQ(_score(cards1).Compare(_score(cards2)), -1);
  }
  {
    // Even two-pair => uneven high-card
    const cardarr_t cards1{S2, C2, C3, H3, H4}, cards2{H2, D2, S3, D3, D5};
    EXPECT_EQ(_score(cards1).Compare(_score(cards2)), -1);
  }
  {
    // Even two-pair => even high-card
    const cardarr_t cards1{S2, C2, C3, H3, H4}, cards2{H2, D2, S3, D3, D4};
    EXPECT_EQ(_score(cards1).Compare(_score(cards2)), 0);
  }
  {
    // Uneven three-of-a-kind
    const cardarr_t cards1{D2, C2, S2, H3, H4}, cards2{H2, D3, S3, C3, D4};
    EXPECT_EQ(_score(cards1).Compare(_score(cards2)), -1);
  }
  {
    // Even three-of-a-kind => uneven high-card
    const cardarr_t cards1{D2, C2, S2, H3, H4}, cards2{D2, C2, H2, S3, S5};
    EXPECT_EQ(_score(cards1).Compare(_score(cards2)), -1);
  }
  {
    // Even three-of-a-kind => even high-card
    const cardarr_t cards1{D2, C2, S2, H3, H4}, cards2{D2, C2, H2, S3, S4};
    EXPECT_EQ(_score(cards1).Compare(_score(cards2)), 0);
  }
  {
    // Uneven straight
    const cardarr_t cards1{D1, C2, S3, H4, H5}, cards2{H6, D7, S8, C9, D10};
    EXPECT_EQ(_score(cards1).Compare(_score(cards2)), -1);
  }
  {
    // Even straight
    const cardarr_t cards1{D1, C2, D3, C4, D5}, cards2{C1, D2, C3, D4, C5};
    EXPECT_EQ(_score(cards1).Compare(_score(cards2)), 0);
  }
  {
    // Uneven flush
    const cardarr_t cards1{D1, D2, D3, D4, D6}, cards2{D1, D2, D3, D4, D7};
    EXPECT_EQ(_score(cards1).Compare(_score(cards2)), -1);
  }
  {
    // Uneven full-house
    const cardarr_t cards1{D1, C1, H1, D4, H4}, cards2{D1, H1, S1, D7, S7};
    EXPECT_EQ(_score(cards1).Compare(_score(cards2)), -1);
  }
  {
    // Even full-house
    const cardarr_t cards1{D1, C1, H1, D4, H4}, cards2{D1, H1, S1, C4, S4};
    EXPECT_EQ(_score(cards1).Compare(_score(cards2)), 0);
  }
  {
    // Uneven four-of-a-kind
    const cardarr_t cards1{D1, C1, H1, S1, H4}, cards2{D1, D4, C4, S4, H4};
    EXPECT_EQ(_score(cards1).Compare(_score(cards2)), -1);
  }
  {
    // Uneven straight-flush
    const cardarr_t cards1{D1, D2, D3, D4, D5}, cards2{D2, D3, D4, D5, D6};
    EXPECT_EQ(_score(cards1).Compare(_score(cards2)), -1);
  }
}
