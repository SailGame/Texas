#include <sailgame/common/game_manager.h>
#include <sailgame/common/provider_msg_builder.h>

#include <tcore/StateMachine.h>

using Core::ProviderMsg;
using Core::RegisterArgs;
using SailGame::Common::EventLoop;
using SailGame::Common::ProviderGameManager;
using SailGame::Common::ProviderMsgBuilder;
using SailGame::Common::ProviderNetworkInterface;
using SailGame::Texas::StateMachine;

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "You must input core server addr: ip:port" << std::endl;
    return -1;
  }
  std::string endpoint(argv[1]);
  auto stub = ProviderNetworkInterface::CreateStub(endpoint);
  ProviderGameManager gameManager(EventLoop::Create(), StateMachine::Create(),
                                  ProviderNetworkInterface::Create(stub));
  gameManager.StartWithRegisterArgs(
      ProviderMsgBuilder::CreateRegisterArgs(0, "texas", "TEXAS", 8, 2));
  return 0;
}
