#include "Backdoor.h"
#include <gtest/gtest.h>
#include <tcore/Dummy.h>

namespace SailGame {
namespace Texas {
namespace Test {

TEST(LiquidationTest, Common) {
  {
    Dummy dm;
    DummyBackdoor dbd(dm);
    auto &players = dbd.GetPlayerStat();
    auto &winners = dbd.GetWinners();

    ASSERT_EQ(dm.Join("a"), 1);
    ASSERT_EQ(dm.Join("b"), 2);
    ASSERT_EQ(dm.Join("c"), 3);
    ASSERT_EQ(dm.Join("d"), 4);
    ASSERT_EQ(dbd.CountPlayers(), 4);

    ASSERT_EQ(dm.Topup(1, 10), 10);
    ASSERT_EQ(dm.Topup(2, 10), 10);
    ASSERT_EQ(dm.Topup(3, 10), 10);
    ASSERT_EQ(dm.Topup(4, 10), 10);

    players.at(1).roundbets = 5;
    players.at(2).roundbets = 5;
    players.at(3).roundbets = 5;
    players.at(4).roundbets = 5;
    winners.push_back(1);

    dbd.Liquidate();
    ASSERT_EQ(players.at(1).bankroll, 25);
    ASSERT_EQ(players.at(2).bankroll, 5);
    ASSERT_EQ(players.at(3).bankroll, 5);
    ASSERT_EQ(players.at(4).bankroll, 5);
  }
  {
    Dummy dm;
    DummyBackdoor dbd(dm);
    auto &players = dbd.GetPlayerStat();
    auto &winners = dbd.GetWinners();

    ASSERT_EQ(dm.Join("a"), 1);
    ASSERT_EQ(dm.Join("b"), 2);
    ASSERT_EQ(dm.Join("c"), 3);
    ASSERT_EQ(dm.Join("d"), 4);
    ASSERT_EQ(dbd.CountPlayers(), 4);

    ASSERT_EQ(dm.Topup(1, 10), 10);
    ASSERT_EQ(dm.Topup(2, 10), 10);
    ASSERT_EQ(dm.Topup(3, 10), 10);
    ASSERT_EQ(dm.Topup(4, 10), 10);

    players.at(1).roundbets = 1;
    players.at(2).roundbets = 2;
    players.at(3).roundbets = 5;
    players.at(4).roundbets = 5;
    winners.push_back(4);

    dbd.Liquidate();
    ASSERT_EQ(players.at(1).bankroll, 9);
    ASSERT_EQ(players.at(2).bankroll, 8);
    ASSERT_EQ(players.at(3).bankroll, 5);
    ASSERT_EQ(players.at(4).bankroll, 18);
  }
}

TEST(LiquidationTest, Allin) {
  {
    Dummy dm;
    DummyBackdoor dbd(dm);
    auto &players = dbd.GetPlayerStat();
    auto &winners = dbd.GetWinners();

    ASSERT_EQ(dm.Join("a"), 1);
    ASSERT_EQ(dm.Join("b"), 2);
    ASSERT_EQ(dm.Join("c"), 3);
    ASSERT_EQ(dm.Join("d"), 4);
    ASSERT_EQ(dbd.CountPlayers(), 4);

    ASSERT_EQ(dm.Topup(1, 10), 10);
    ASSERT_EQ(dm.Topup(2, 10), 10);
    ASSERT_EQ(dm.Topup(3, 5), 5);
    ASSERT_EQ(dm.Topup(4, 10), 10);

    players.at(1).roundbets = 6;
    players.at(2).roundbets = 6;
    players.at(3).roundbets = 5;
    players.at(4).roundbets = 6;
    winners.push_back(3);

    dbd.Liquidate();
    ASSERT_EQ(players.at(1).bankroll, 5);
    ASSERT_EQ(players.at(2).bankroll, 5);
    ASSERT_EQ(players.at(3).bankroll, 20);
    ASSERT_EQ(players.at(4).bankroll, 5);
  }
}

TEST(LiquidationTest, Multiwinners) {
  {
    Dummy dm;
    DummyBackdoor dbd(dm);
    auto &players = dbd.GetPlayerStat();
    auto &winners = dbd.GetWinners();

    ASSERT_EQ(dm.Join("a"), 1);
    ASSERT_EQ(dm.Join("b"), 2);
    ASSERT_EQ(dm.Join("c"), 3);
    ASSERT_EQ(dm.Join("d"), 4);
    ASSERT_EQ(dbd.CountPlayers(), 4);

    ASSERT_EQ(dm.Topup(1, 10), 10);
    ASSERT_EQ(dm.Topup(2, 10), 10);
    ASSERT_EQ(dm.Topup(3, 10), 10);
    ASSERT_EQ(dm.Topup(4, 10), 10);

    players.at(1).roundbets = 5;
    players.at(2).roundbets = 5;
    players.at(3).roundbets = 5;
    players.at(4).roundbets = 5;
    winners.push_back(2);
    winners.push_back(3);

    dbd.Liquidate();
    ASSERT_EQ(players.at(1).bankroll, 5);
    ASSERT_EQ(players.at(2).bankroll, 15);
    ASSERT_EQ(players.at(3).bankroll, 15);
    ASSERT_EQ(players.at(4).bankroll, 5);
  }
}

TEST(LiquidationTest, MultiwinnersWithAllin) {
  {
    Dummy dm;
    DummyBackdoor dbd(dm);
    auto &players = dbd.GetPlayerStat();
    auto &winners = dbd.GetWinners();

    ASSERT_EQ(dm.Join("a"), 1);
    ASSERT_EQ(dm.Join("b"), 2);
    ASSERT_EQ(dm.Join("c"), 3);
    ASSERT_EQ(dm.Join("d"), 4);
    ASSERT_EQ(dbd.CountPlayers(), 4);

    ASSERT_EQ(dm.Topup(1, 10), 10);
    ASSERT_EQ(dm.Topup(2, 3), 3);
    ASSERT_EQ(dm.Topup(3, 10), 10);
    ASSERT_EQ(dm.Topup(4, 10), 10);

    players.at(1).roundbets = 6;
    players.at(2).roundbets = 3;
    players.at(3).roundbets = 6;
    players.at(4).roundbets = 6;
    winners.push_back(2);
    winners.push_back(3);

    dbd.Liquidate();
    ASSERT_EQ(players.at(1).bankroll, 4);
    ASSERT_EQ(players.at(2).bankroll, 7);
    ASSERT_EQ(players.at(3).bankroll, 18);
    ASSERT_EQ(players.at(4).bankroll, 4);
  }
}

} // namespace Test
} // namespace Texas
} // namespace SailGame