#include <sailgame_pb/core/error.pb.h>
#include <sailgame_pb/core/provider.pb.h>
#include <sailgame_pb/texas/texas.pb.h>

#include <sailgame/common/provider_msg_builder.h>
#include <sailgame/common/state_machine.h>
#include <sailgame/common/util.h>

#include "StateMachine.h"

namespace SailGame {
namespace Texas {

ProviderMsgs StateMachine::Transition(const RegisterRet &msg) {
  if (msg.err() != Core::ErrorNumber::OK) {
    exit(-1);
  }
  return {};
}

ProviderMsgs StateMachine::Transition(const StartGameArgs &msg) {
  ProviderMsgs msgs;
  if (mState.mRooms.count(msg.roomid()) > 0) {
    std::cout << "Recv StartGame on same room twice" << std::endl;
    return msgs;
  }
  auto &room = mState.mRooms[msg.roomid()];
  for (uint32_t uid : msg.userid()) {
    // addr is unused
    room.Join("");
  }
  room.Begin();
  DumpFullState(msgs, msg.roomid());
  return msgs;
}

ProviderMsgs StateMachine::Transition(const QueryStateArgs &msg) {
  ProviderMsgs msgs;
  DumpFullState(msgs, msg.roomid());
  return msgs;
}

ProviderMsgs StateMachine::Transition(const UserOperationArgs &msg) {
  ProviderMsgs msgs;
  if (mState.mRooms.count(msg.roomid()) == 0) {
    std::cout << "Recv UserOperationArgs on unkown roomid" << std::endl;
    return msgs;
  }
  // a simple hack: the core uid is 0-based, texas uid is 1-based
  const texas_defines::uid_t userid = msg.userid() + 1;
  auto &room = mState.mRooms.at(msg.roomid());
  const UserOperation userOperation =
      Common::Util::UnpackGrpcAnyTo<UserOperation>(msg.custom());
  switch (userOperation.Operation_case()) {
  case UserOperation::OperationCase::kBet:
    room.Play(userid, userOperation.bet().points());
  case UserOperation::OperationCase::kCheck:
    room.Play(userid, 0);
  case UserOperation::OperationCase::kDrop:
  case UserOperation::OperationCase::kExit:
    room.Play(userid, -1);
  }
  DumpFullState(msgs, msg.roomid());
  return msgs;
}

void StateMachine::DumpFullState(ProviderMsgs &msgs, int32_t roomId) {
  const auto &room = mState.mRooms.at(roomId);
  for (size_t i = room.FirstPlayer(); i <= room.LastPlayer(); i++) {
    Core::ProviderMsg msg = msgs.emplace_back();
    auto args = msg.mutable_notifymsgargs();
    args->set_err(Core::ErrorNumber::OK);
    args->set_roomid(roomId);
    args->set_userid(i - 1);

    Texas::NotifyMsg notifyMsg;
    auto *state = notifyMsg.mutable_state();
    DummyBackdoor::DumpStateToGrpc(room, i, *state);

    args->mutable_custom()->PackFrom(notifyMsg);
  }
}

} // namespace Texas
} // namespace SailGame