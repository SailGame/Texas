#include "Backdoor.h"
#include <gtest/gtest.h>

TEST(InteractionTest, FromInitToPreflop) {
  Dummy dm;
  DummyBackdoor dbd(dm);
  const auto &players = dbd.GetPlayerStat();

  // Check init status.
  ASSERT_EQ(dbd.CountPlayers(), 0);

  ASSERT_EQ(dm.Join("a"), 1);
  ASSERT_EQ(dm.Join("b"), 2);

  // Check the restriction of player number's low bound.
  ASSERT_EQ(dbd.CountPlayers(), 2);
  ASSERT_EQ(dm.Begin(), Dummy::INVALID_PLAYER_NUM);

  ASSERT_EQ(dm.Join("c"), 3);
  ASSERT_EQ(dm.Join("d"), 4);

  ASSERT_EQ(dbd.CountPlayers(), 4);

  ASSERT_EQ(dm.Play(1, 0), Dummy::STOP);
  ASSERT_EQ(dm.Play(2, 0), Dummy::STOP);
  ASSERT_EQ(dm.Play(3, 0), Dummy::STOP);
  ASSERT_EQ(dm.Play(4, 0), Dummy::STOP);

  // Can not start game without top-up.
  ASSERT_EQ(dm.Begin(), Dummy::STOP);

  ASSERT_EQ(dm.Play(1, 0), Dummy::STOP);
  ASSERT_EQ(dm.Play(2, 0), Dummy::STOP);
  ASSERT_EQ(dm.Play(3, 0), Dummy::STOP);
  ASSERT_EQ(dm.Play(4, 0), Dummy::STOP);

  ASSERT_EQ(dm.Topup(1, 1), 1);
  ASSERT_EQ(dm.Topup(2, 2), 2);
  ASSERT_EQ(dm.Topup(3, 10), 10);
  ASSERT_EQ(dm.Topup(4, 10), 10);

  // Ensure a correct preflop.
  ASSERT_EQ(dm.Begin(), Dummy::READY);
  for (const auto &ent : players)
    ASSERT_EQ(ent.second.holecards.size(), 2);
  ASSERT_EQ(dbd.GetBoard().size(), 0);

  // Assume that game initialize with the player of uid 1 being the button.
  // Thus, small-blind is 2, big-blind is 3, the next player to bet is 4.
  ASSERT_EQ(players.at(2).roundbets, 1);
  ASSERT_EQ(players.at(3).roundbets, 2);
  ASSERT_EQ(dbd.GetCurrentChips(), 2);

  ASSERT_EQ(dm.Play(1, 0), Dummy::NOT_YOUR_TURN);
  ASSERT_EQ(dm.Play(2, 0), Dummy::NOT_YOUR_TURN);
  ASSERT_EQ(dm.Play(3, 0), Dummy::NOT_YOUR_TURN);
  ASSERT_EQ(dm.Play(4, 0), Dummy::INVALID_BET);
  ASSERT_EQ(dm.Play(4, 1), Dummy::INVALID_BET);
  ASSERT_EQ(dm.Play(4, 2), Dummy::READY);
}

// By giving a certain deck to check the following process.
// Remeber the next card comes from the tail of the deck array.
// TEST(InteractionTest, Episode1) {
//   dm.SetDeck({

// }
