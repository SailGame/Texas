#include <gtest/gtest.h>
#include <tcore/StateMachine.h>

namespace SailGame {
namespace Texas {
namespace Test {

TEST(StateMachineTest, StartGame) {
  StateMachine sm;
  ProviderMsg input;
  int roomID = 1;
  int userNum = 3;
  auto *startGameMsg = input.mutable_startgameargs();
  startGameMsg->set_roomid(roomID);
  for (size_t i = 0; i < userNum; i++) {
    startGameMsg->add_userid(i);
  }

  const auto &msgs = sm.TransitionForProviderMsg(input);

  EXPECT_EQ(msgs.size(), userNum);
}

} // namespace Test
} // namespace Texas
} // namespace SailGame