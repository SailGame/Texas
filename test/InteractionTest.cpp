#include "Backdoor.h"
#include <gtest/gtest.h>

TEST(InteractionTest, FromInitToPreflop) {
  Dummy dm;
  DummyBackdoor dbd(dm);

  // Check init status.
  EXPECT_EQ(dbd.CountPlayers(), 0);

  EXPECT_EQ(dm.Join("a"), 1);
  EXPECT_EQ(dm.Join("b"), 2);

  // Check the restriction of player number's low bound.
  EXPECT_EQ(dbd.CountPlayers(), 2);
  EXPECT_EQ(dm.Begin(), Dummy::INVALID_PLAYER_NUM);

  EXPECT_EQ(dm.Join("c"), 3);
  EXPECT_EQ(dm.Join("d"), 4);

  EXPECT_EQ(dbd.CountPlayers(), 4);

  EXPECT_EQ(dm.Play(1, 0), Dummy::STOP);
  EXPECT_EQ(dm.Play(2, 0), Dummy::STOP);
  EXPECT_EQ(dm.Play(3, 0), Dummy::STOP);
  EXPECT_EQ(dm.Play(4, 0), Dummy::STOP);

  // Can not start game without top-up.
  EXPECT_EQ(dm.Begin(), Dummy::STOP);

  EXPECT_EQ(dm.Play(1, 0), Dummy::STOP);
  EXPECT_EQ(dm.Play(2, 0), Dummy::STOP);
  EXPECT_EQ(dm.Play(3, 0), Dummy::STOP);
  EXPECT_EQ(dm.Play(4, 0), Dummy::STOP);

  EXPECT_EQ(dm.Topup(1, 1), 1);
  EXPECT_EQ(dm.Topup(2, 2), 2);
  EXPECT_EQ(dm.Topup(3, 10), 10);
  EXPECT_EQ(dm.Topup(4, 10), 10);

  // Ensure a correct preflop.
  EXPECT_EQ(dm.Begin(), Dummy::READY);
  for (const auto &ent : dbd.GetHolecards())
    EXPECT_EQ(ent.second.size(), 2);
  EXPECT_EQ(dbd.GetBoard().size(), 0);

  // Assume that game initialize with the player of uid 1 being the button.
  // Thus, small-blind is 2, big-blind is 3, the next player to bet is 4.
  EXPECT_EQ(dbd.GetRoundbets().at(2), 1);
  EXPECT_EQ(dbd.GetRoundbets().at(3), 2);
  EXPECT_EQ(dbd.GetCurrentChips(), 2);

  EXPECT_EQ(dm.Play(1, 0), Dummy::NOT_YOUR_TURN);
  EXPECT_EQ(dm.Play(2, 0), Dummy::NOT_YOUR_TURN);
  EXPECT_EQ(dm.Play(3, 0), Dummy::NOT_YOUR_TURN);
  EXPECT_EQ(dm.Play(4, 0), Dummy::INVALID_BET);
  EXPECT_EQ(dm.Play(4, 1), Dummy::INVALID_BET);
  EXPECT_EQ(dm.Play(4, 2), Dummy::READY);
}

// By giving a certain deck to check the following process.
// Remeber the next card comes from the tail of the deck array.
// TEST(InteractionTest, Episode1) {
//   dm.SetDeck({

// }
