#include "Backdoor.h"
#include <gtest/gtest.h>

TEST(InteractionTest, Interactions) {
  Dummy dm;
  DummyBackdoor dbd(dm);

  EXPECT_EQ(dbd.CountPlayers(), 0);

  EXPECT_EQ(dm.Join("a"), 1);
  EXPECT_EQ(dm.Join("b"), 2);

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

  EXPECT_GT(dm.Topup(1, 1), 0);
  EXPECT_GT(dm.Topup(2, 2), 0);
  EXPECT_GT(dm.Topup(1, 10), 0);
  EXPECT_GT(dm.Topup(1, 10), 0);

  EXPECT_EQ(dm.Begin(), Dummy::READY);

  // Assume that game initialize with the player of uid 1 being the button.
  // Thus, small-blind is 2, big-blind is 3, the next player to bet is 4.
}
