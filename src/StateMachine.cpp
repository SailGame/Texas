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
  // auto [userIdToInitHandcards, flippedCard, firstPlayer] = mState.NewGame(
  //     msg.roomid(), Util::ConvertGrpcRepeatedFieldToVector(msg.userid()),
  //     Util::UnpackGrpcAnyTo<StartGameSettings>(msg.custom()));

  ProviderMsgs msgs;
  // for (const auto &entry : userIdToInitHandcards) {
  //     auto gameStart = MsgBuilder::CreateGameStart(
  //         entry.second, flippedCard, firstPlayer);
  //     auto msgToSend = ProviderMsgBuilder::CreateNotifyMsgArgs(
  //         0, Core::ErrorNumber::OK, msg.roomid(), entry.first, gameStart);
  //     msgs.push_back(msgToSend);
  // }
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