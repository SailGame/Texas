use tokio_stream::StreamExt;
use tonic::{transport::{Channel, Uri}, Streaming};

use super::core_proto::{game_core_client::GameCoreClient, ProviderMsg, provider_msg, RegisterArgs, GameSetting};

pub struct TexasProviderConfig {
    m_id: String,
    m_core_address: Uri
}

pub struct TexasProvider {
    m_config: TexasProviderConfig,
    m_client: GameCoreClient<Channel>,
    m_sender: flume::Sender<ProviderMsg>,
    m_recver: Streaming<ProviderMsg>,
    m_seq: i32,
}

impl TexasProvider {
    pub async fn new(config: TexasProviderConfig) -> TexasProvider {
        let core_addr = config.m_core_address.clone();
        let mut client = GameCoreClient::connect(core_addr).await.unwrap();
        let (tx, rx) = flume::unbounded::<ProviderMsg>();

        let resp_stream = client.provider(rx.into_stream()).await.unwrap().into_inner();
        TexasProvider{
            m_config: config,
            m_client: client,
            m_sender: tx,
            m_recver: resp_stream,
            m_seq: 0
        }
    }

    pub fn register(&mut self) -> Result<(), Box<dyn std::error::Error>> {
        let seq = self.get_seq();
        self.m_sender.send(ProviderMsg {
            sequence_id: seq,
            msg: Some(provider_msg::Msg::RegisterArgs(
                RegisterArgs{
                    id: self.m_config.m_id.clone(),
                    game_name: "Texas".to_string(),
                    game_setting: Some(GameSetting{
                        max_users: 9,
                        min_users: 2,
                    })
                }
            )),
        })?;
        Ok(())
    }

    pub fn poll(&mut self) -> Result<(), Box<dyn std::error::Error>> {
        while true {
            let msg = self.m_recver.next();
        };
        Ok(())
    }

    fn get_seq(&mut self) -> i32 {
        self.m_seq += 1;
        self.m_seq
    }
}