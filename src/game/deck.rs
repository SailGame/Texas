use std::cmp::Ordering;
use std::collections::HashMap;

use crate::game::card::Card;
use crate::game::card::Dealer;
use crate::game::player::*;
use serde::{Deserialize, Serialize};

use super::ranking::calculate_player_hand_score;

pub static MAX_NUM_OF_PLAYERS: usize = 10;

#[derive(Copy, Clone, PartialEq, Eq)]
pub enum PlayerAction {
    BET(usize),
    FOLD,
    SHOWHAND,
}

#[derive(Copy, Clone, PartialEq, Eq)]
pub enum GameState {
    WAITING,
    PLAYING,
    SETTLE,
}

#[derive(Clone, Copy, Serialize, Deserialize)]
pub struct DeckConfig {
    m_small_blind: usize,
    m_big_blind: usize,
}

impl DeckConfig {
    pub fn default() -> DeckConfig {
        return DeckConfig {
            m_small_blind: 1,
            m_big_blind: 2,
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
    m_executor: Option<usize>,
    m_pos_of_next_round: Option<usize>,
    m_min_raise: usize,
    m_round_bet: usize,
    m_state: GameState
}

impl Deck {
    pub fn new() -> Deck {
        let mut deck = Deck {
            m_config: DeckConfig::default(),
            m_players: vec![Player::default(); MAX_NUM_OF_PLAYERS],
            m_cards: Vec::new(),
            m_dealer: Dealer::new(),
            m_button: 0,
            m_sb: 0,
            m_bb: 0,
            m_executor: None,
            m_pos_of_next_round: None,
            m_min_raise: 0,
            m_round_bet: 0,
            m_state: GameState::WAITING,
        };

        return deck;
    }

    pub fn update_config(&self, config: &DeckConfig) -> Result<(), String> {
        return Ok(());
    }

    pub fn add_player(&mut self, id: u32, pos: usize, chip: usize) -> Result<(), String> {
        if pos > self.m_players.len() {
            return Err(format!("Invalid position ({pos}). Valid Range 0 - {}", self.m_players.len() - 1));
        }
        if chip < self.m_config.m_big_blind {
            return Err(format!("Invalid chip ({chip}). Big blind {}", self.m_config.m_big_blind));
        }

        if self.get_player(id).is_ok() {
            return Err(format!("Player id ({id}) is used"));
        }

        let player = &mut self.m_players[pos];

        if !player.is_empty() {
            return Err(format!("Position ({pos}) is used"));
        }

        player.init(id, chip);
        return Ok(());
    }

    pub fn update_player(&mut self, id: u32, chip: usize) -> Result<(), String> {
        if self.m_state != GameState::WAITING {
            return Err("Invalid GameState".into());
        }
        let pos = self.get_player(id)?;
        let player = &mut self.m_players[pos];
        player.m_chip = chip;

        return Ok(());
    }

    fn get_num_of_players(&self, state: PlayerState) -> usize {
        self.m_players.iter().filter(|p| !p.is_empty() && p.m_state == state).count()
    }

    pub fn start(&mut self) -> Result<(), String> {
        if self.m_state != GameState::WAITING {
            return Err("Invalid deck state".into());
        }
        if self.get_num_of_players(PlayerState::WAITING) < 2 {
            return Err("Invalid num of players".into());
        }

        self.set_player_state_playing();
        self.init_button_and_blind();
        self.init_cards();
        self.m_state = GameState::PLAYING;

        self.m_pos_of_next_round = self.m_executor;

        return Ok(());
    }

    pub fn act(&mut self, id: u32, action: PlayerAction) -> Result<(), String> {
        if self.m_state != GameState::PLAYING {
            return Err("Invalid GameState".into());
        }
        let pos = self.get_player(id)?;
        if self.m_executor.is_none() || pos != self.m_executor.unwrap() {
            return Err(format!("Not your turn. Executor: {:?}", self.m_executor));
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

                    self.m_pos_of_next_round.replace(pos);
                    self.m_round_bet = bet_chip;
                }

                player.bet(num);
            }
            PlayerAction::FOLD => {
                player.m_state = PlayerState::FOLD;
            }
            PlayerAction::SHOWHAND => {
                return Err("Not implement".into());
            }
        }

        self.m_executor = self.calc_executor(self.m_executor.unwrap(), true);

        return Ok(());
    }

    pub fn deal(&mut self) -> Result<(), String>{
        if self.m_executor.is_some() {
            return Err("not your turn, dealer".into());
        }
        if self.m_state != GameState::PLAYING {
            return Err("Invalid GameState".into());
        }
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
                return Ok(self.settle());
            }
            _ => {
                panic!("wtf");
            }
        }

        if self.get_num_of_players(PlayerState::PLAYING) >= 2 {
            self.m_pos_of_next_round = None;
            self.m_executor = self.calc_executor(self.m_sb, false);
            self.m_pos_of_next_round = self.m_executor;
        }
        self.m_round_bet = 0;
        self.m_min_raise = self.m_bb;

        self.m_players.iter_mut().for_each(|p| p.round_reset());

        Ok(())
    }

    fn set_player_state_playing(&mut self) {
        self.m_players.iter_mut().for_each(|p| 
            if !p.is_empty() && p.m_state == PlayerState::WAITING {
                p.m_state = PlayerState::PLAYING;
        })
    }

    fn init_button_and_blind(&mut self) {
        self.m_button = self.calc_executor(self.m_button, true).unwrap();
        self.m_sb = self.calc_executor(self.m_button, true).unwrap();
        self.m_bb = self.calc_executor(self.m_sb, true).unwrap();
        self.m_min_raise = self.m_config.m_big_blind;
        self.m_round_bet = self.m_config.m_big_blind;

        let sb = &mut self.m_players[self.m_sb];
        sb.bet(self.m_config.m_small_blind.min(sb.m_chip));

        let bb = &mut self.m_players[self.m_bb];
        bb.bet(self.m_config.m_big_blind.min(bb.m_chip));

        self.m_executor = self.calc_executor(self.m_bb, true);
        assert!(self.m_executor.is_some());
    }

    fn init_cards(&mut self) {
        self.m_dealer.shuffle();

        let mut exec = self.calc_executor(self.m_sb, false).unwrap();

        for _i in 0..self.get_num_of_players(PlayerState::PLAYING) {
            let player = &mut self.m_players[exec];
            assert!(player.m_cards.is_empty());
            player.m_cards.push(self.m_dealer.eject_card(false));
            player.m_cards.push(self.m_dealer.eject_card(false));

            exec = self.calc_executor(exec, true).unwrap();
        }
        assert_eq!(exec, self.m_sb);

        assert!(self.m_cards.is_empty());
    }

    fn settle(&mut self) -> () {
        assert!(self.m_cards.len() == 5);
        self.m_state = GameState::SETTLE;

        self.m_players.iter_mut().for_each(|p| {
            if !p.m_cards.is_empty() {
                p.m_hand_score.replace(calculate_player_hand_score(&p.m_cards, &self.m_cards));
            }
        });

        // chip pool
        let mut chips = Vec::new();
        for pos in 0..self.m_players.len() {
            let p = &mut self.m_players[pos];
            if p.m_total_bet_chip != 0 {
                assert!(p.m_chip >= p.m_total_bet_chip);
                p.m_chip -= p.m_total_bet_chip;
                chips.push((p.m_total_bet_chip, pos));
            }
        }
        chips.sort_unstable();

        let mut pools = Vec::new();
        let mut max_bet = 0;
        for (chip, player_pos) in chips {
            if chip > max_bet {
                let pool_bet = chip - max_bet;
                pools.push((pool_bet, 0, Vec::new()));
            }
            max_bet = chip;

            let player = &self.m_players[player_pos];
            for (pool_bet, pool_size, player_poss) in &mut pools {
                *pool_size = *pool_bet + *pool_size;
                if player.is_playing() {
                    player_poss.push(player_pos);
                }
            }
        }

        // settle
        let mut winners = HashMap::new();
        for (_, pool_size, player_poss) in &mut pools {
            assert!(!player_poss.is_empty());
            player_poss.sort_by(|a, b| {
                // desc order
                self.m_players[*b].cmp_hand_score(&self.m_players[*a])
            });

            let mut loser_delimiter = player_poss.len();
            for i in 1..player_poss.len() {
                let a = &self.m_players[player_poss[i-1]];
                let b = &self.m_players[player_poss[i]];
                if a.cmp_hand_score(b) == Ordering::Greater {
                    loser_delimiter = i;
                    break;
                }
            }

            let win_bet = *pool_size / loser_delimiter;
            let mut rest_bet = *pool_size % loser_delimiter;
            assert!(win_bet * loser_delimiter + rest_bet == *pool_size);
            for i in 0..loser_delimiter {
                let player_pos = player_poss[i];
                winners.insert(player_pos, win_bet + rest_bet + winners.get(&player_pos).unwrap_or(&0));
                rest_bet = 0;
            }
        }

        
        for (winner_pos, win_bet) in winners {
            let player = &mut self.m_players[winner_pos];
            player.m_chip += win_bet;
        }

    }

    fn reset(&mut self) {
        self.m_state = GameState::WAITING;
        self.m_cards.clear();
        self.m_players.iter_mut().for_each(|p| {
            if p.m_chip > 0 {
                p.reset();
            }
            else {
                *p = Player::default();
            }
        })
    }

    fn calc_executor(&self, cur: usize, next: bool) -> Option<usize> {
        let n_player = self.m_players.len();
        if next {
            return self.calc_executor(cur + 1, false);
        } else {
            for n in 0..n_player {
                let pos = (cur + n) % n_player;
                let player = &self.m_players[pos];
                if player.is_empty() {
                    continue;
                }
                else if self.m_pos_of_next_round.unwrap_or(usize::MAX) == pos {
                    return None;
                }
                else if player.is_playing() && player.m_chip > player.m_total_bet_chip {
                    return Some(pos);
                }
            }
        }
        panic!("wtf");
    }

    fn get_player(&mut self, id: u32) -> Result<usize, String> {
        let mut pos: usize = 0;
        for p in &mut self.m_players {
            if p.m_id == id {
                return Ok(pos);
            }
            pos += 1;
        }
        return Err("Can't find player".into());
    }

    pub fn get_players(&self) -> &Vec<Player> {
        &self.m_players
    }

    pub fn get_public_cards(&self) -> &Vec<Card> {
        &self.m_cards
    }

    pub fn get_executor(&self) -> Option<u32> {
        return Some(self.m_players[self.m_executor?].m_id);
    }
}


#[cfg(test)]
mod tests {
    use crate::game::{deck::{Deck, PlayerAction}};

    #[test]
    fn init_deck() {
        let mut deck = Deck::new();

        let p1 = 1;
        assert!(deck.add_player(p1, 0, 100).is_ok());
        assert!(deck.start().is_err());

        let p2 = 2;
        assert!(deck.add_player(p1, 1, 200).is_err());
        assert!(deck.add_player(p2, 0, 200).is_err());
        assert!(deck.add_player(p2, 1, 200).is_ok());

        assert_eq!(deck.m_players[0].m_cards.is_empty(), true);

        assert!(deck.start().is_ok());

        assert!(deck.m_executor.unwrap() == 0);
        assert!(deck.m_button == 1);
        assert!(deck.m_sb == 0);
        assert!(deck.m_bb == 1);

        assert_eq!(deck.m_players[0].m_cards.is_empty(), false);
    }

    #[test]
    fn settle_1v1() {
        let mut deck = Deck::new();

        let p1 = 1;
        assert_eq!(deck.add_player(p1, 0, 100), Ok(()));

        let p2 = 2;
        assert_eq!(deck.add_player(p2, 1, 200), Ok(()));

        assert_eq!(deck.start(), Ok(()));
        assert_ne!(deck.deal(), Ok(()));

        assert_eq!(deck.act(p1, PlayerAction::BET(20)), Ok(()));
        assert_eq!(deck.act(p2, PlayerAction::BET(19)), Ok(()));
        assert_eq!(deck.deal(), Ok(()));

        assert_eq!(deck.act(p1, PlayerAction::BET(0)), Ok(()));
        assert_eq!(deck.act(p2, PlayerAction::BET(0)), Ok(()));
        assert_eq!(deck.deal(), Ok(()));

        assert_eq!(deck.act(p1, PlayerAction::BET(0)), Ok(()));
        assert_eq!(deck.act(p2, PlayerAction::BET(0)), Ok(()));
        assert_eq!(deck.deal(), Ok(()));

        assert_eq!(deck.act(p1, PlayerAction::BET(0)), Ok(()));
        assert_eq!(deck.act(p2, PlayerAction::BET(0)), Ok(()));
        assert_eq!(deck.deal(), Ok(()));

        assert_ne!(deck.deal(), Ok(()));
    }
}