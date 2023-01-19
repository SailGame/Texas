use tonic::transport::Channel;

use super::core_proto::game_core_client::GameCoreClient;

pub struct TexasProviderConfig {
    m_core_address: String
}

pub struct TexasProvider {
    m_config: TexasProviderConfig,
    m_client: GameCoreClient<Channel>,
}

impl TexasProvider {
    fn connect(&mut self) {
        let mut client = GameCoreClient::connect(self.m_config.m_core_address);
    }
}