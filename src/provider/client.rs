use std::collections::HashMap;

use prost::bytes::{BytesMut, BufMut};
use tokio_stream::StreamExt;
use tonic::{transport::{Channel, Uri}, Streaming, codec::{Codec, Encoder, Decoder, DecodeBuf}};

use crate::game::{log::DeckLogger, deck::Deck};

use super::{core_proto, texas_proto};
use super::core_proto::{game_core_client::GameCoreClient, provider_msg};

struct TexasGame {
    m_deck: Deck
}

pub struct TexasProviderConfig {
    pub m_id: String,
    pub m_core_address: Uri
}

pub struct TexasProvider {
    m_config: TexasProviderConfig,
    m_client: GameCoreClient<Channel>,
    m_sender: flume::Sender<core_proto::ProviderMsg>,
    m_recver: Streaming<core_proto::ProviderMsg>,
    m_games: HashMap<i32, TexasGame>,
    m_seq: i32,
}

impl TexasProvider {
    pub async fn new(config: TexasProviderConfig) -> TexasProvider {
        let core_addr = config.m_core_address.clone();
        let mut client = GameCoreClient::connect(core_addr).await.unwrap();
        let (tx, rx) = flume::unbounded::<core_proto::ProviderMsg>();
        let reg = tx.send_async(core_proto::ProviderMsg {
            sequence_id: 0,
            msg: Some(provider_msg::Msg::RegisterArgs(
                core_proto::RegisterArgs{
                    id: config.m_id.clone(),
                    game_name: "Texas".to_string(),
                    game_setting: Some(core_proto::GameSetting{
                        max_users: 9,
                        min_users: 2,
                    })
                }
            )),
        });

        let mut resp_stream = client.provider(rx.into_stream()).await.unwrap().into_inner();

        reg.await.unwrap();

        let ret_msg = resp_stream.next().await.unwrap().unwrap().msg.unwrap();
        if let provider_msg::Msg::RegisterRet(reg_ret) = ret_msg {
            let err = core_proto::ErrorNumber::from_i32(reg_ret.err).unwrap();
            if err != core_proto::ErrorNumber::Ok {
                panic!("Register Err {}", err.as_str_name());
            }
        }
        else
        {
            panic!("Get Register Ret Failed. Msg {:?}", ret_msg);
        }

        TexasProvider{
            m_config: config,
            m_client: client,
            m_sender: tx,
            m_recver: resp_stream,
            m_games: HashMap::new(),
            m_seq: 1
        }
    }


    pub async fn poll(&mut self) {
        loop {
            let msg = self.m_recver.next().await;
            
        };
    }

    fn get_seq(&mut self) -> i32 {
        self.m_seq += 1;
        self.m_seq
    }

    fn handle_msg(&mut self, msg: core_proto::ProviderMsg) {
        if let Some(body) = msg.msg {
            match body {
                provider_msg::Msg::StartGameArgs(args) => self.start_game(args),
                provider_msg::Msg::QueryStateArgs(args) => self.query_state(args),
                provider_msg::Msg::UserOperationArgs(args) => self.user_operation(args),
                unhandled_msg => panic!("Unexpected msg {:?}", unhandled_msg)
            }
        }
    }

    fn start_game(&mut self, msg: core_proto::StartGameArgs) {
        let codec = tonic::codec::ProstCodec::<texas_proto::StartGameSettings, texas_proto::StartGameSettings>::default();

        if let Some(custom_msg) = msg.custom {
            let bytes = BytesMut::new();
            bytes.put(&custom_msg.value);
            let buf = DecodeBuf::from(custom_msg.value.);
            let mut texas_config = codec.decoder().decode(&mut buf);
        }
        self.m_games.insert(msg.room_id, msg.)
        todo!()
    }

    fn query_state(&mut self, msg: core_proto::QueryStateArgs) {
        todo!()
    }

    fn user_operation(&mut self, msg: core_proto::UserOperationArgs) {
        todo!()
    }

}


impl DeckLogger for TexasProvider {
    fn publish_deck_state(&mut self, state: &crate::game::log::DeckState) {
        todo!()
    }

    fn publish_private_player_state(&mut self, state: &crate::game::log::PrivatePlayerState) {
        todo!()
    }

    fn publish_settle_state(&mut self, state: &crate::game::log::SettleState) {
        todo!()
    }
}