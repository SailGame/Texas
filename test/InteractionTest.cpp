#include "Backdoor.h"
#include <gtest/gtest.h>

TEST(InteractionTest, Interactions) {
  Dummy dm;
  DummyBackdoor dbd;

  EXPECT_EQ(dbd.CountPlayers(dm), 0);

  EXPECT_EQ(dm.Join("a"), 1);
  EXPECT_EQ(dm.Join("b"), 2);
  EXPECT_EQ(dm.Join("c"), 3);
  EXPECT_EQ(dm.Join("d"), 4);

  EXPECT_EQ(dbd.CountPlayers(dm), 4);

  EXPECT_EQ(dm.Play(1, 0), Dummy::STOP);
  EXPECT_EQ(dm.Play(2, 0), Dummy::STOP);
  EXPECT_EQ(dm.Play(3, 0), Dummy::STOP);
  EXPECT_EQ(dm.Play(4, 0), Dummy::STOP);

  EXPECT_EQ(dm.Begin(), Dummy::READY);

  EXPECT_EQ(dm.Play(1, 0), Dummy::INVALID_BET);
  EXPECT_EQ(dm.Play(2, 0), Dummy::NOT_YOUR_TURN);
  EXPECT_EQ(dm.Play(3, 0), Dummy::NOT_YOUR_TURN);
  EXPECT_EQ(dm.Play(4, 0), Dummy::NOT_YOUR_TURN);
}
