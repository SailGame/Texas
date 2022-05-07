use crate::game::card::Card;
use crate::game::card::Dealer;
use crate::game::player::*;

use std::cmp;

#[derive(Copy, Clone)]
pub enum PlayerAction {
    CHECK,
    BET(usize),
    FOLD,
    SHOWHAND,
}

pub enum DeckState {
    PAUSE,
    WAITING,
    PLAYING,
}

pub struct DeckConfig {
    m_small_blind: i32,
    m_big_blind: i32,
    m_timeout_secs: i32,
}

impl DeckConfig {
    pub fn default() -> DeckConfig {
        return DeckConfig {
            m_small_blind: 1,
            m_big_blind: 2,
            m_timeout_secs: 30,
        };
    }
}

pub struct Deck {
    m_config: DeckConfig,
    m_players: Vec<Player>,
    m_cards: Vec<Card>,
    m_dealer: Dealer,
    m_button: usize,
    m_sb: usize,
    m_bb: usize,
    m_executor: usize,
    m_last_executor: usize,
    m_last_bet: usize,
    m_state: DeckState,
}

impl Deck {
    pub fn new(max_players: usize) -> Deck {
        let mut deck = Deck {
            m_config: DeckConfig::default(),
            m_players: vec![Player::empty(); max_players],
            m_cards: Vec::new(),
            m_dealer: Dealer::new(),
            m_button: 0,
            m_sb: 0,
            m_bb: 0,
            m_executor: 0,
            m_last_executor: 0,
            m_last_bet: 0,
            m_state: DeckState::WAITING,
        };

        return deck;
    }

    pub fn update_config(&self, config: &DeckConfig) -> Result<String, String> {
        return Ok("".into());
    }

    pub fn update_player(
        &mut self,
        pos: usize,
        name: String,
        chip: usize,
        status: PlayerState,
    ) -> Result<String, String> {
        if pos > self.m_players.len() {
            return Err("".into());
        }

        return Ok("".into());
    }

    pub fn start(&mut self) -> Result<String, String> {
        let mut available_players: i32 = 0;
        for p in &self.m_players {
            match p.m_state.m_deck {
                PlayerDeckState::ACTIVE => available_players += 1,
                PlayerDeckState::WAITING => available_players += 1,
                _ => (),
            }
        }
        if available_players < 2 {
            return Err("".into());
        }

        for p in &mut self.m_players {
            match p.m_state.m_deck {
                PlayerDeckState::ACTIVE => p.m_state.m_game = PlayerGameState::PLAYING,
                PlayerDeckState::WAITING => {
                    p.m_state.m_deck = PlayerDeckState::ACTIVE;
                    p.m_state.m_game = PlayerGameState::PLAYING;
                }
                // big blind
                _ => (),
            }
        }

        self.init_button_and_blind();
        self.init_cards();
        self.reset_timer();
        self.m_state = DeckState::PLAYING;

        return Ok("".into());
    }

    pub fn stop(&self) -> Result<String, String> {
        return Ok("".into());
    }

    pub fn act(
        &mut self,
        pos: usize,
        name: String,
        action: PlayerAction,
    ) -> Result<String, String> {
        if pos != self.m_executor {
            return Err("".into());
        }
        let player = &mut self.m_players[pos];

        match action {
            PlayerAction::BET(num) => {
                let remaining_chip = player.m_chip - player.m_bet_chip;
                if num != remaining_chip && num < self.m_last_bet {
                    return Err("".into());
                }
                if num > remaining_chip {
                    return Err("".into());
                }

                player.m_bet_chip += num;
                self.m_last_bet = cmp::max(num, self.m_last_bet);
                self.m_last_executor = pos;
            }
            PlayerAction::CHECK => (),
            PlayerAction::FOLD => (),
            PlayerAction::SHOWHAND => (),
        }

        self.m_executor = self.get_executor(self.m_executor, true).unwrap();

        return Ok("".into());
    }

    fn init_button_and_blind(&mut self) {
        self.m_button = self.get_executor(self.m_button, true).unwrap();
        self.m_sb = self.get_executor(self.m_button, true).unwrap();
        self.m_bb = self.get_executor(self.m_sb, true).unwrap();
        self.m_executor = self.get_executor(self.m_bb, true).unwrap();
        self.m_last_executor = self.m_executor;
    }

    fn init_cards(&mut self) {
        self.m_dealer.shuffle();

        self.m_players[self.m_sb].m_cards[0] = self.m_dealer.eject_card(false);
        self.m_players[self.m_sb].m_cards[1] = self.m_dealer.eject_card(false);

        let mut exec = self.get_executor(self.m_sb, true).unwrap();

        while exec != self.m_sb {
            self.m_players[exec].m_cards[0] = self.m_dealer.eject_card(false);
            self.m_players[exec].m_cards[1] = self.m_dealer.eject_card(false);
            exec = self.get_executor(exec, true).unwrap();
        }
    }

    fn reset_timer(&mut self) {}

    fn timeout(&mut self) {}

    fn reset(&mut self) {}

    fn get_executor(&self, cur: usize, next: bool) -> Result<usize, String> {
        let n_player = self.m_players.len();
        if next {
            return self.get_executor(((cur + 1) % n_player), false);
        } else {
            for n in 0..n_player {
                match self.m_players[(cur + n) % n_player].m_state.m_game {
                    PlayerGameState::PLAYING => return Ok((cur + n) % n_player),
                    _ => (),
                }
            }
            return Err("".into());
        }
    }
}
