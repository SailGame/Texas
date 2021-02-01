#pragma once

#include <memory>
#include <sailgame/common/state_machine.h>
#include <sailgame/common/types.h>
#include <sailgame_pb/core/provider.pb.h>
#include <sailgame_pb/texas/texas.pb.h>

#include "Dummy.h"

namespace SailGame {
namespace Texas {

using Common::IState;
using Common::ProviderMsgBuilder;
using Common::ProviderMsgs;
using Common::ProviderStateMachine;
using ::Core::CloseGameArgs;
using ::Core::ProviderMsg;
using ::Core::QueryStateArgs;
using ::Core::RegisterRet;
using ::Core::StartGameArgs;
using ::Core::UserOperationArgs;
using ::Texas::NotifyMsg;
using ::Texas::StartGameSettings;
using ::Texas::UserOperation;

class StateMachine : public ProviderStateMachine {
public:
  StateMachine() = default;

  static std::shared_ptr<ProviderStateMachine> Create() {
    return std::make_shared<StateMachine>();
  }

  const IState &GetState() const override { return mState; }

protected:
  virtual ProviderMsgs Transition(const RegisterRet &msg) override;

  virtual ProviderMsgs Transition(const StartGameArgs &msg) override;

  virtual ProviderMsgs Transition(const QueryStateArgs &msg) override;

  virtual ProviderMsgs Transition(const UserOperationArgs &msg) override;

private:
  ProviderMsgs Transition(const UserOperation &msg);

private:
  Dummy mState;
};
} // namespace Texas
} // namespace SailGame