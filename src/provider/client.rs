use tonic::transport::{Channel, Uri};

use super::core_proto::{game_core_client::GameCoreClient, ProviderMsg};
use tokio_stream::{StreamExt, Stream};

pub struct TexasProviderConfig {
    m_core_address: Uri
}

pub struct TexasProvider {
    m_config: TexasProviderConfig,
    m_client: GameCoreClient<Channel>,
    m_seq: i32,
}

impl TexasProvider {
    pub async fn new(config: TexasProviderConfig) -> TexasProvider {
        let core_addr = config.m_core_address.clone();
        TexasProvider{
            m_config: config,
            m_client: GameCoreClient::connect(core_addr).await.unwrap(),
            m_seq: 0
        }
    }

    async fn register(&mut self) -> Result<(), Box<dyn std::error::Error>> {
        let in_stream = tokio_stream::iter(1..i32::MAX).map(|i| ProviderMsg {
            sequence_id: i,
            msg: None,
        });

        let out_stream = self.m_client.provider().await?.into_inner();
        Ok(())
    }

    fn get_seq(&mut self) -> i32 {
        self.m_seq += 1;
        self.m_seq
    }
}

impl Stream for TexasProvider {
    type Item;

    fn poll_next(self: std::pin::Pin<&mut Self>, cx: &mut std::task::Context<'_>) -> std::task::Poll<Option<Self::Item>> {
        todo!()
    }
}