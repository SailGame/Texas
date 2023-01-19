use crate::game::card::Card;
use crate::game::card::Dealer;
use crate::game::log::PrivatePlayerState;
use crate::game::player::*;
use serde::{Deserialize, Serialize};

use super::log::{DeckLogger, DeckState};
use super::ranking::calculate_player_hand_score;

#[derive(Copy, Clone, PartialEq, Eq)]
pub enum PlayerAction {
    BET(usize),
    FOLD,
    SHOWHAND,
}

#[derive(Copy, Clone, PartialEq, Eq)]
pub enum GameState {
    WAITING,
    PAUSE,
    PLAYING,
}

#[derive(Clone, Copy, Serialize, Deserialize)]
pub struct DeckConfig {
    m_small_blind: usize,
    m_big_blind: usize,
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
    m_pos_of_next_round: usize,
    m_min_raise: usize,
    m_round_bet: usize,
    m_state: GameState,
    m_logger: Box<dyn DeckLogger>
}

impl Deck {
    pub fn new(max_players: usize, logger: Box<dyn DeckLogger>) -> Deck {
        let mut deck = Deck {
            m_config: DeckConfig::default(),
            m_players: vec![Player::empty(); max_players],
            m_cards: Vec::new(),
            m_dealer: Dealer::new(),
            m_button: 0,
            m_sb: 0,
            m_bb: 0,
            m_executor: 0,
            m_pos_of_next_round: 0,
            m_min_raise: 0,
            m_round_bet: 0,
            m_state: GameState::WAITING,
            m_logger: logger
        };

        return deck;
    }

    pub fn update_config(&self, config: &DeckConfig) -> Result<String, String> {
        return Ok("".into());
    }

    pub fn add_player(&mut self, name: &String, pos: usize, chip: usize) -> Result<(), String> {
        if pos > self.m_players.len() {
            return Err(format!("Invalid position ({pos}). Valid Range 0 - {}", self.m_players.len() - 1));
        }
        if chip < self.m_config.m_big_blind {
            return Err(format!("Invalid chip ({chip}). Big blind {}", self.m_config.m_big_blind));
        }

        if self.get_player(name).is_ok() {
            return Err(format!("Player name ({name}) is used"));
        }

        let player = &mut self.m_players[pos];

        if !player.is_empty() {
            return Err(format!("Position ({pos}) is used"));
        }

        player.init(name, chip);
        return Ok(());
    }

    pub fn update_player(&mut self, name: &String, chip: usize) -> Result<String, String> {
        if self.m_state != GameState::WAITING {
            return Err("Invalid GameState".into());
        }
        let pos = self.get_player(name)?;
        let player = &mut self.m_players[pos];
        player.m_chip = chip;

        return Ok("".into());
    }

    fn get_num_of_players(&self, state: PlayerGameState) -> usize {
        self.m_players.iter().filter(|p| !p.is_empty() && p.m_state.m_game == state).count()
    }

    pub fn disconnect(&mut self, name: &String) -> Result<String, String> {
        let pos = self.get_player(name)?;
        let player = &mut self.m_players[pos];
        player.m_state.m_conn = PlayerConnectionState::DISCONNECTED;
        return Ok("".into());
    }

    pub fn start(&mut self) -> Result<(), String> {
        if self.m_state != GameState::WAITING {
            return Err("Invalid deck state".into());
        }
        if self.get_num_of_players(PlayerGameState::WAITING) < 2 {
            return Err("Invalid num of players".into());
        }

        self.set_player_state_playing();
        self.init_button_and_blind();
        self.init_cards();
        self.reset_timer();
        self.m_state = GameState::PLAYING;

        self.m_logger.publish_deck_state(&DeckState{
            config: self.m_config,
            players: vec![],
            executor: self.m_executor,
            timeout: 0,
        });

        return Ok(());
    }

    pub fn act(&mut self, name: &String, action: PlayerAction) -> Result<(), String> {
        let pos = self.get_player(name)?;
        if pos != self.m_executor {
            return Err("Not your turn".into());
        }
        let player = &mut self.m_players[pos];
        match action {
            PlayerAction::BET(num) => {
                let remaining_chip = player.m_chip - player.m_total_bet_chip;
                let bet_chip = num + player.m_round_bet_chip;
                if num == 0 {
                    if self.m_round_bet != 0 {
                        return Err("Someone raised, can't check".into());
                    }
                }
                else if num > remaining_chip {
                    return Err("Input bet is larger than remaining chips".into());
                }
                else if bet_chip < self.m_round_bet {
                    if num != remaining_chip {
                        return Err(format!("Not enough chips to call. user bet {bet_chip} Current round bet {}", self.m_round_bet));
                    }
                    // All In
                }
                else if bet_chip == self.m_round_bet {
                    // Call
                }
                else if bet_chip > self.m_round_bet {
                    // Raise
                    let raise = bet_chip - self.m_round_bet;
                    if raise >= self.m_min_raise {
                        self.m_min_raise = raise;
                    }
                    else if num != remaining_chip {
                        return Err("Raise is less than min raise".into());
                    }

                    self.m_pos_of_next_round = pos;
                    self.m_round_bet = bet_chip;
                }

                player.bet(num);
            }
            PlayerAction::FOLD => {
                player.m_state.m_game = PlayerGameState::FOLD;
            }
            PlayerAction::SHOWHAND => {
                return Err("Not implement".into());
            }
        }

        let move_pos_of_next_round = pos == self.m_pos_of_next_round && player.m_state.m_game != PlayerGameState::PLAYING;
        let next_executor = self.get_executor(self.m_executor, true);

        if next_executor.is_ok() {
            self.m_executor = next_executor.unwrap();
            if self.m_executor == self.m_pos_of_next_round {
                if self.get_num_of_players(PlayerGameState::PLAYING) >= 2 {
                    self.deal();
                }
                else {
                    self.settle();
                }
            }
        }
        else {
            self.settle()
        }

        if move_pos_of_next_round {
            self.m_pos_of_next_round = self.m_executor;
        }

        return Ok(());
    }

    fn set_player_state_playing(&mut self) {
        self.m_players.iter_mut().for_each(|p| 
            if !p.is_empty() && p.m_state.m_game == PlayerGameState::WAITING {
                p.m_state.m_game = PlayerGameState::PLAYING;
        })
    }

    fn init_button_and_blind(&mut self) {
        self.m_button = self.get_executor(self.m_button, true).unwrap();
        self.m_sb = self.get_executor(self.m_button, true).unwrap();
        self.m_bb = self.get_executor(self.m_sb, true).unwrap();
        self.m_min_raise = self.m_config.m_big_blind;
        self.m_round_bet = self.m_config.m_big_blind;

        let sb = &mut self.m_players[self.m_sb];
        sb.bet(self.m_config.m_small_blind);

        let bb = &mut self.m_players[self.m_bb];
        bb.bet(self.m_config.m_big_blind);

        self.m_executor = self.get_executor(self.m_bb, true).unwrap();
        self.m_pos_of_next_round = self.m_executor;
    }

    fn init_cards(&mut self) {
        self.m_dealer.shuffle();

        let mut exec = self.get_executor(self.m_sb, false).unwrap();

        for _i in 0..self.get_num_of_players(PlayerGameState::PLAYING) {
            let player = &mut self.m_players[exec];
            player.m_cards.clear();
            player.m_cards.push(self.m_dealer.eject_card(false));
            player.m_cards.push(self.m_dealer.eject_card(false));

            self.m_logger.publish_private_player_state(&PrivatePlayerState{
                name: player.m_name.clone(),
                cards: player.m_cards.clone()
            });

            exec = self.get_executor(exec, true).unwrap();
        }
        assert_eq!(exec, self.m_sb);

        self.m_cards.clear();
    }

    fn deal(&mut self) {
        match self.m_cards.len() {
            0 => {
                self.m_cards.push(self.m_dealer.eject_card(false));
                self.m_cards.push(self.m_dealer.eject_card(false));
                self.m_cards.push(self.m_dealer.eject_card(true));
            }
            3 => {
                self.m_cards.push(self.m_dealer.eject_card(true));
            }
            4 => {
                self.m_cards.push(self.m_dealer.eject_card(true));
            }
            5 => {
                self.settle();
                return;
            }
            _ => {
                panic!("wtf");
            }
        }

        self.m_executor = self.get_executor(self.m_sb, false).unwrap();
        self.m_pos_of_next_round = self.m_executor;
        self.m_round_bet = 0;
        self.m_min_raise = self.m_bb;

        self.m_players.iter_mut().for_each(|p| p.round_clear());
    }

    fn settle(&mut self) {
        while self.m_cards.len() < 5 {
            self.deal();
        }

        self.m_players.iter_mut().for_each(|p| {
            if p.m_cards.len() == 2 {
                p.m_hand_score.replace(calculate_player_hand_score(&p.m_cards, &self.m_cards));
            }
        });
    }

    fn reset_timer(&mut self) {}

    fn timeout(&mut self) {}

    fn reset(&mut self) {}

    fn get_executor(&self, cur: usize, next: bool) -> Result<usize, String> {
        let n_player = self.m_players.len();
        if next {
            return self.get_executor(cur + 1, false);
        } else {
            for n in 0..n_player {
                let pos = (cur + n) % n_player;
                match self.m_players[pos].m_state.m_game {
                    PlayerGameState::PLAYING => return Ok(pos),
                    _ => (),
                }
            }
            return Err("".into());
        }
    }

    fn get_player(&mut self, name: &String) -> Result<usize, String> {
        let mut pos: usize = 0;
        for p in &mut self.m_players {
            if p.m_name == *name {
                return Ok(pos);
            }
            pos += 1;
        }
        return Err("Can't find player".into());
    }
}


#[cfg(test)]
mod tests {
    use crate::game::{deck::{Deck, PlayerAction}, log::{DeckLogger, DeckState, PrivatePlayerState, SettleState}};

    #[derive(Default)]
    struct MockDeckLogger {
        deck_states: Vec<DeckState>,
        private_player_states: Vec<PrivatePlayerState>,
        settle_states: Vec<SettleState>
    }

    impl DeckLogger for MockDeckLogger {
        fn publish_deck_state(&mut self, state: &DeckState) {
            self.deck_states.push(state.clone());
        }

        fn publish_private_player_state(&mut self, state: &PrivatePlayerState) {
            self.private_player_states.push(state.clone());
        }

        fn publish_settle_state(&mut self, state: &SettleState) {
            self.settle_states.push(state.clone());
        }
    }
    #[test]
    fn init_deck() {
        let mut mock_logger = Box::new(MockDeckLogger::default());
        let mut deck = Deck::new(8, mock_logger);

        let p1 = "jack".to_string();
        assert!(deck.add_player(&p1, 0, 100).is_ok());
        assert!(deck.start().is_err());

        let p2 = "jerry".to_string();
        assert!(deck.add_player(&p1, 1, 200).is_err());
        assert!(deck.add_player(&p2, 0, 200).is_err());
        assert!(deck.add_player(&p2, 1, 200).is_ok());

        assert_eq!(deck.m_players[0].m_cards.is_empty(), true);

        assert!(deck.start().is_ok());

        assert!(deck.m_executor == 0);
        assert!(deck.m_button == 1);
        assert!(deck.m_sb == 0);
        assert!(deck.m_bb == 1);

        assert_eq!(deck.m_players[0].m_cards.is_empty(), false);
    }

    #[test]
    fn settle_1v1() {
        let mut mock_logger = Box::new(MockDeckLogger::default());
        let mut deck = Deck::new(8, mock_logger);

        let p1 = "jack".to_string();
        assert_eq!(deck.add_player(&p1, 0, 100), Ok(()));

        let p2 = "jerry".to_string();
        assert_eq!(deck.add_player(&p2, 1, 200), Ok(()));

        assert_eq!(deck.start(), Ok(()));

        assert_eq!(deck.act(&p1, PlayerAction::BET(20)), Ok(()));
        assert_eq!(deck.act(&p2, PlayerAction::BET(19)), Ok(()));

        assert_eq!(deck.act(&p1, PlayerAction::BET(0)), Ok(()));
        assert_eq!(deck.act(&p2, PlayerAction::BET(0)), Ok(()));

        assert_eq!(deck.act(&p1, PlayerAction::BET(0)), Ok(()));
        assert_eq!(deck.act(&p2, PlayerAction::BET(0)), Ok(()));

        assert_eq!(deck.act(&p1, PlayerAction::BET(0)), Ok(()));
        assert_eq!(deck.act(&p2, PlayerAction::BET(0)), Ok(()));
    }
}