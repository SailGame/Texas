#include <sailgame/common/game_manager.h>
#include <sailgame/common/provider_msg_builder.h>

#include "state_machine.h"

using Core::ProviderMsg;
using Core::RegisterArgs;
using SailGame::Texas::StateMachine;
using SailGame::Common::ProviderGameManager;
using SailGame::Common::EventLoop;
using SailGame::Common::ProviderNetworkInterface;
using SailGame::Common::ProviderMsgBuilder;

int main(int argc, char** argv) {
    std::string endpoint = "localhost:50051";
    auto stub = ProviderNetworkInterface::CreateStub(endpoint);
    ProviderGameManager gameManager(
        EventLoop::Create(),
        StateMachine::Create(),
        ProviderNetworkInterface::Create(stub));
    gameManager.StartWithRegisterArgs(
        ProviderMsgBuilder::CreateRegisterArgs(0, "texas", "TEXAS", 8, 2));
    return 0;
}