use std::{collections::HashMap};

use log::{Level, log};
use prost::Message;
use tokio::sync::mpsc;
use tokio_stream::StreamExt;
use tonic::{transport::{Channel, Uri}, Streaming};

use crate::game::{deck::{Deck, MAX_NUM_OF_PLAYERS, PlayerAction}, card::Card, player::Player};

use super::{texas_proto};
use super::{core_proto};
use super::core_proto::{game_core_client::GameCoreClient, provider_msg};

struct User {
    m_buyin: usize,
    m_pos: Option<usize>,
}

struct TexasGame {
    m_users: HashMap<u32, User>,
    m_deck: Deck
}

impl TexasGame {
    fn new(users: Vec<u32>) -> TexasGame {
        let mut users_map = HashMap::new();
        for user in users {
            users_map.insert(user, User{
                m_buyin: 200,
                m_pos: None
            });
        }

        return TexasGame { m_users: users_map, m_deck: Deck::new() }
    }
}

pub struct TexasProviderConfig {
    pub m_id: String,
    pub m_core_address: Uri
}

pub struct TexasProvider {
    m_config: TexasProviderConfig,
    m_client: GameCoreClient<Channel>,
    m_sender: mpsc::Sender<core_proto::ProviderMsg>,
    m_recver: Streaming<core_proto::ProviderMsg>,
    m_games: HashMap<i32, TexasGame>,
}

fn to_grpc_card(card: &Card) -> texas_proto::Card {
    let mut grpc_card = texas_proto::Card::default();
    match card.m_color {
        crate::game::card::CardColor::SPADE => grpc_card.color = texas_proto::card::Color::Spade as i32,
        crate::game::card::CardColor::HEART => grpc_card.color = texas_proto::card::Color::Heart as i32,
        crate::game::card::CardColor::CLUB => grpc_card.color = texas_proto::card::Color::Club as i32,
        crate::game::card::CardColor::DIAMOND => grpc_card.color = texas_proto::card::Color::Diamond as i32,
    };
    grpc_card
}

fn to_grpc_player(player: &Player, pos: usize) -> texas_proto::UserState {
    let mut user_state = texas_proto::UserState::default();
    if player.is_empty() {
        user_state.userid = 0;
    }
    else {
        user_state.userid = player.m_id;
        user_state.user_pos = pos as u32;
        user_state.round_bet = player.m_round_bet_chip as u32;
        user_state.total_bet = player.m_total_bet_chip as u32;
        user_state.total_chips = player.m_chip as u32;

        match player.m_state {
            crate::game::player::PlayerState::WAITING => user_state.game_state = texas_proto::UserGameState::Playing as i32,
            crate::game::player::PlayerState::PLAYING => user_state.game_state = texas_proto::UserGameState::Playing as i32,
            crate::game::player::PlayerState::FOLD => user_state.game_state = texas_proto::UserGameState::Fold as i32,
        };

        if player.m_show_hand {
            for card in &player.m_cards {
                user_state.cards.push(to_grpc_card(card));
            }
        }
    }
    user_state
}

fn publish_state(sender: &mut mpsc::Sender<core_proto::ProviderMsg>, room_id: i32, game: &TexasGame) {
    let mut game_state = texas_proto::GameState::default();

    let executor = game.m_deck.get_executor();
    let players = game.m_deck.get_players();
    let cards = game.m_deck.get_public_cards();

    game_state.executor = executor.unwrap_or(0);
    game_state.remaing_time = 30;

    for card in cards {
        game_state.deck_cards.push(to_grpc_card(card));
    }
    for pos in 0..players.len() {
        let player = &players[pos];
        game_state.user_states.push(to_grpc_player(player, pos));
    }

    let mut notify_msg = prost_types::Any{
        type_url: "texas/NotifyMsg".into(),
        value: Vec::new(),
    };

    for pos in 0..players.len() {
        let player = &players[pos];

        if !player.m_show_hand {
            let user = &mut game_state.user_states[pos];
            for card in &player.m_cards {
                user.cards.push(to_grpc_card(card));
            }
            game_state.encode(&mut notify_msg.value).unwrap();
            game_state.user_states[pos].clear();
        }

        let msg = core_proto::ProviderMsg {
            sequence_id: 0,
            msg: Some(provider_msg::Msg::NotifyMsgArgs(
                core_proto::NotifyMsgArgs{
                    err: core_proto::ErrorNumber::Ok as i32,
                    room_id: room_id,
                    user_id: player.m_id,
                    custom: Some(notify_msg.clone()),
                }
            )),
        };
        log!(Level::Info, "Provider sent {:?}", msg);

        sender.blocking_send(msg).unwrap();
    }
}

impl TexasProvider {
    pub async fn new(config: TexasProviderConfig) -> TexasProvider {
        let core_addr = config.m_core_address.clone();
        let mut client = GameCoreClient::connect(core_addr).await.unwrap();
        let (tx, mut rx) = mpsc::channel::<core_proto::ProviderMsg>(100);
        
        let reg = tx.send(core_proto::ProviderMsg {
            sequence_id: 0,
            msg: Some(provider_msg::Msg::RegisterArgs(
                core_proto::RegisterArgs{
                    id: config.m_id.clone(),
                    game_name: "Texas".to_string(),
                    game_setting: Some(core_proto::GameSetting{
                        max_users: MAX_NUM_OF_PLAYERS as i32,
                        min_users: 2,
                    })
                }
            )),
        }).await.unwrap();

        let stream = async_stream::stream! {
            while let Some(item) = rx.recv().await {
                yield item;
            }
        };
        let mut resp_stream = client.provider(stream).await.unwrap().into_inner();
        log!(Level::Info, "Provider stream connected");

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

        log!(Level::Info, "Get Register Msg Ret");

        TexasProvider{
            m_config: config,
            m_client: client,
            m_sender: tx,
            m_recver: resp_stream,
            m_games: HashMap::new(),
        }
    }


    pub async fn poll(&mut self) {
        log!(Level::Info, "Provider starts polling");

        loop {
            let msg = self.m_recver.next().await;

            if msg.is_some() {
                self.handle_msg(msg.unwrap().unwrap());
            }
        };
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
        log!(Level::Info, "Provider start game {:?}", msg);

        let mut game = TexasGame::new(msg.user_id);

        let mut pos = 0;
        for (user_id, _) in &game.m_users {
            game.m_deck.add_player(*user_id, pos, 200).unwrap();
            pos += 1;
        }
        game.m_deck.start().unwrap();
        publish_state(&mut self.m_sender, msg.room_id, &game);

        self.m_games.insert(msg.room_id, game);
    }

    fn query_state(&mut self, msg: core_proto::QueryStateArgs) {
        todo!()
    }

    fn user_operation(&mut self, msg: core_proto::UserOperationArgs) {
        log!(Level::Info, "Provider recv user operation {:?}", msg);

        if let Some(game) = self.m_games.get_mut(&msg.room_id) {
            let texas_operation = <texas_proto::UserOperation as prost::Message>::decode(msg.custom.unwrap().value.as_slice()).unwrap();
            match texas_operation.operation.unwrap() {
                texas_proto::user_operation::Operation::Bet(bet) => {
                    if game.m_deck.act(msg.user_id, PlayerAction::BET(bet.chips as usize)).is_ok() {
                        publish_state(&mut self.m_sender, msg.room_id, &game);
                    }
                }
                texas_proto::user_operation::Operation::Fold(_) => {
                    if game.m_deck.act(msg.user_id, PlayerAction::FOLD).is_ok() {
                        publish_state(&mut self.m_sender, msg.room_id, &game);
                    };
                },
                texas_proto::user_operation::Operation::Query(_) => todo!(),
            }

            while game.m_deck.deal().is_ok() {
                publish_state(&mut self.m_sender, msg.room_id, &game);
            }
        }
    }
}
