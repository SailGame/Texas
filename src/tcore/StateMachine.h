#pragma once

#include <memory>
#include <sailgame/common/state_machine.h>
#include <sailgame/common/types.h>
#include <sailgame_pb/core/provider.pb.h>
#include <sailgame_pb/texas/texas.pb.h>

#include "Dummy.h"
#include "State.h"

namespace SailGame {
namespace Texas {

using Common::IState;
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
using GrpcState = ::Texas::State;

class DummyBackdoor {
public:
  DummyBackdoor() = delete;
  // Getters
  static void DumpStateToGrpc(const Dummy &dm, texas_defines::uid_t targetUid,
                              GrpcState &grpcState) {
    grpcState.set_playernum(dm.players.size());
    for (size_t i = 0; i < dm.board.size(); i++) {
      auto *card = grpcState.add_deckcards();
      card->set_color(
          static_cast<::Texas::Card_Color>(poker::GetCardColor(dm.board[i])));
      card->set_num(poker::GetCardNum(dm.board[i]));
    }
    for (const auto &[uid, player] : dm.players) {
      auto *playerStat = grpcState.add_userstates();
      playerStat->set_userid(uid - 1);
      playerStat->set_totalpoints(player.bankroll);
      playerStat->set_roundbet(player.roundbets);
      if (targetUid == uid) {
        for (texas_defines::card_t internalCard : player.holecards) {
          auto *card = playerStat->add_cards();
          card->set_color(static_cast<::Texas::Card_Color>(
              poker::GetCardColor(internalCard)));
          card->set_num(poker::GetCardNum(internalCard));
        }
      }
    }
  }
};

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
  void DumpFullState(ProviderMsgs &msgs, int32_t roomId);

private:
  State mState;
};
} // namespace Texas
} // namespace SailGame