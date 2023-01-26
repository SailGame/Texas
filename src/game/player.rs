use std::cmp::Ordering;

use crate::game::card::Card;
use serde::{Deserialize, Serialize};

use super::ranking::CardScore;

#[derive(Copy, Clone, PartialEq, Eq, Deserialize, Serialize)]
pub enum PlayerState {
    WAITING,
    PLAYING,
    FOLD,
}

#[derive(Clone)]
pub struct Player {
    pub m_id: u32,
    pub m_chip: usize,
    pub m_round_bet_chip: usize,
    pub m_total_bet_chip: usize,
    pub m_cards: Vec<Card>,
    pub m_show_hand: bool,
    pub m_hand_score: Option<CardScore>,
    pub m_state: PlayerState,
}

impl Player {
    pub fn default() -> Player {
        return Player {
            m_id: 0,
            m_chip: 0,
            m_round_bet_chip: 0,
            m_total_bet_chip: 0,
            m_cards: Vec::new(),
            m_show_hand: false,
            m_hand_score: None,
            m_state: PlayerState::WAITING,
        };
    }

    pub fn is_empty(&self) -> bool {
        return self.m_id == 0;
    }

    pub fn init(&mut self, id: u32, chip: usize) {
        self.m_id = id;
        self.m_chip = chip;
        self.m_state = PlayerState::WAITING;
    }

    pub fn reset(&mut self) {
        self.m_round_bet_chip = 0;
        self.m_total_bet_chip = 0;
        self.m_cards.clear();
        self.m_hand_score = None;
        self.m_state = PlayerState::WAITING;
    }

    pub fn round_reset(&mut self) {
        self.m_round_bet_chip = 0;
    }

    pub fn bet(&mut self, num: usize) {
        self.m_round_bet_chip += num;
        self.m_total_bet_chip += num;
    }

    pub fn show_hand(&mut self) {
        self.m_show_hand = true;
    }

    pub fn is_playing(&self) -> bool {
        match self.m_state {
            PlayerState::PLAYING => true,
            _ => false
        }
    }

    pub fn cmp_hand_score(&self, right: &Player) -> Ordering {
        let left = &self.m_hand_score.as_ref().unwrap().m_type;
        let right = &right.m_hand_score.as_ref().unwrap().m_type;
        if *left == *right {
            return Ordering::Equal;
        }
        else if *left < *right {
            return Ordering::Less;
        }
        else {
            return Ordering::Greater;
        }
    }
}
