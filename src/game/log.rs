use serde::{Deserialize, Serialize};

use super::{card::Card, player::PlayerState, deck::DeckConfig};

#[derive(Clone, Serialize, Deserialize)]
pub struct PublicPlayerState {
    pub name: String,
    pub chip: usize,
    pub total_bet_chip: usize,
    pub round_bet_chip: usize,
    pub state: PlayerState,
}

#[derive(Clone, Serialize, Deserialize)]
pub struct DeckState {
    pub config: DeckConfig,
    pub players: Vec<PublicPlayerState>,
    pub executor: usize,
    pub timeout: usize,
}

#[derive(Clone, Serialize, Deserialize)]
pub struct PrivatePlayerState {
    pub name: String,
    pub cards: Vec<Card>
}

#[derive(Clone, Serialize, Deserialize)]
pub struct SettleState {
    pub name: String,
    pub hand_cards: Vec<Card>,
    pub cards: Vec<Card>,
    pub chip_change: i32,
}

pub trait DeckLogger {
    fn publish_deck_state(&mut self, state: &DeckState);
    fn publish_private_player_state(&mut self, state: &PrivatePlayerState);
    fn publish_settle_state(&mut self, state: &SettleState);
}