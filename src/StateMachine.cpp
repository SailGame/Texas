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
  return msgs;
}

ProviderMsgs StateMachine::Transition(const QueryStateArgs &msg) { return {}; }

ProviderMsgs StateMachine::Transition(const UserOperationArgs &msg) {
  // mState.mCurrentRoomId = msg.roomid();
  // mState.mCurrentUserId = msg.userid();
  return Transition(Common::Util::UnpackGrpcAnyTo<UserOperation>(msg.custom()));
}

ProviderMsgs StateMachine::Transition(const UserOperation &msg) {
  switch (msg.Operation_case()) {
    /// TODO: handle other operations
  }
  throw std::runtime_error("Unsupported msg type");
  return {};
}

} // namespace Texas
} // namespace SailGame