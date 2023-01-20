use crate::game::card::Card;
use serde::{Deserialize, Serialize};

use super::ranking::CardScore;

#[derive(Copy, Clone, PartialEq, Eq, Deserialize, Serialize)]
pub enum PlayerGameState {
    WAITING,
    PLAYING,
    ALLIN,
    FOLD,
}

#[derive(Copy, Clone, Deserialize, Serialize)]
pub struct PlayerState {
    pub m_game: PlayerGameState,
}

impl PlayerState {
    pub fn empty() -> PlayerState {
        return PlayerState {
            m_game: PlayerGameState::WAITING,
        };
    }
}

#[derive(Clone)]
pub struct Player {
    pub m_name: String,
    pub m_chip: usize,
    pub m_round_bet_chip: usize,
    pub m_total_bet_chip: usize,
    pub m_cards: Vec<Card>,
    pub m_hand_score: Option<CardScore>,
    pub m_state: PlayerState,
}

impl Player {
    pub fn empty() -> Player {
        return Player {
            m_name: "".to_string(),
            m_chip: 0,
            m_round_bet_chip: 0,
            m_total_bet_chip: 0,
            m_cards: Vec::new(),
            m_hand_score: None,
            m_state: PlayerState::empty(),
        };
    }

    pub fn is_empty(&self) -> bool {
        return self.m_name.is_empty();
    }

    pub fn init(&mut self, name: &String, chip: usize) {
        self.m_name = name.clone();
        self.m_chip = chip;
        self.m_state = PlayerState {
            m_game: PlayerGameState::WAITING,
        }
    }

    pub fn clear(&mut self) {
        self.m_name.clear();
        self.m_chip = 0;
        self.m_round_bet_chip = 0;
        self.m_total_bet_chip = 0;
        self.m_cards.clear();
        self.m_hand_score = None;
    }

    pub fn round_clear(&mut self) {
        self.m_round_bet_chip = 0;
    }

    pub fn bet(&mut self, num: usize) {
        self.m_round_bet_chip += num;
        self.m_total_bet_chip += num;

        if self.m_total_bet_chip == self.m_chip {
            self.m_state.m_game = PlayerGameState::ALLIN;
        }
    }
}
