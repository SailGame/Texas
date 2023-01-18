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
    m_last_act_player: usize,
    m_min_raise: usize,
    m_round_bet: usize,
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
            m_last_act_player: 0,
            m_min_raise: 0,
            m_round_bet: 0,
            m_state: DeckState::PLAYING,
        };

        return deck;
    }

    pub fn update_config(&self, config: &DeckConfig) -> Result<String, String> {
        return Ok("".into());
    }

    pub fn add_player(&mut self, name: &String, pos: usize, chip: usize) -> Result<String, String> {
        if pos > self.m_players.len() {
            return Err("Invalid position".into());
        }
        if chip == 0 {
            return Err("Invalid chip".into());
        }

        if self.get_player(name).is_ok() {
            return Err("Player name is used".into());
        }

        let player = &mut self.m_players[pos];

        if !player.is_empty() {
            return Err("Position is used".into());
        }

        player.init(name, chip);
        return Ok("".into());
    }

    pub fn update_player(&mut self, name: &String, chip: usize) -> Result<String, String> {
        let pos = self.get_player(name)?;
        let player = &mut self.m_players[pos];
        player.m_chip = chip;

        return Ok("".into());
    }

    fn get_num_of_players(&self) -> usize {
        self.m_players.iter().filter(|p| !p.is_empty()).count()
    }

    pub fn disconnect(&mut self, name: &String) -> Result<String, String> {
        let pos = self.get_player(name)?;
        let player = &mut self.m_players[pos];
        player.m_state.m_conn = PlayerConnectionState::DISCONNECTED;
        return Ok("".into());
    }

    pub fn start(&mut self) -> Result<String, String> {
        if self.get_num_of_players() < 2 {
            return Err("Invalid num of players".into());
        }

        self.set_player_state_playing();
        self.init_button_and_blind();
        self.init_cards();
        self.reset_timer();
        self.m_state = DeckState::PLAYING;

        return Ok("".into());
    }

    pub fn act(&mut self, name: &String, action: PlayerAction) -> Result<String, String> {
        let pos = self.get_player(name)?;
        if pos != self.m_executor {
            return Err("Not your turn".into());
        }
        let player = &mut self.m_players[pos];
        match action {
            PlayerAction::BET(num) => {
                let remaining_chip = player.m_chip - player.m_total_bet_chip;
                let bet_chip = num + player.m_bet_chip;
                if num > remaining_chip || num == 0 {
                    return Err("Not enough chips".into());
                }
                else if bet_chip < self.m_round_bet {
                    if num != remaining_chip {
                        return Err("Not enough chips".into());
                    }
                    // All In
                }
                else if bet_chip == self.m_round_bet {
                    // Call
                }
                else if bet_chip > self.m_round_bet {
                    if bet_chip - self.m_round_bet >= self.m_min_raise {
                        // Raise
                        let raise : usize = bet_chip - self.m_round_bet;

                        self.m_min_raise = raise * 2;
                        self.m_last_act_player = pos;
                        self.m_round_bet = bet_chip;
                    }
                    else {
                        return Err("Raise is less than min raise".into());
                    }
                }

                player.m_bet_chip += num;
                player.m_total_bet_chip += num;

                if player.m_total_bet_chip == player.m_chip {
                    player.m_state.m_game = PlayerGameState::ALLIN;
                }
            }
            PlayerAction::CHECK => {
                if self.m_round_bet != 0 {
                    return Err("Someone raised, can't check".into());
                }
            }
            PlayerAction::FOLD => {
                player.m_state.m_game = PlayerGameState::FOLD;
            }
            PlayerAction::SHOWHAND => {
                return Err("Not implement".into());
            }
        }

        self.m_executor = self.get_executor(self.m_executor, true).unwrap();

        if self.m_executor == self.m_last_act_player {
            self.deal()
        }

        return Ok("".into());
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
        self.m_min_raise = self.m_bb;
        self.m_round_bet = 0;
        self.m_executor = self.get_executor(self.m_bb, true).unwrap();
        self.m_last_act_player = self.m_executor;
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
            }
            _ => {
                panic!("");
            }
        }

        self.m_executor = self.get_executor(self.m_sb, false).unwrap();
        self.m_last_act_player = self.m_executor;
        self.m_round_bet = 0;
        self.m_min_raise = self.m_bb;

        self.m_players.iter_mut().for_each(|p| p.m_bet_chip = 0);
    }

    fn settle(&mut self) {

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
    use crate::game::{deck::Deck, card::Card};

    #[test]
    fn init_deck() {
        let mut deck = Deck::new(8);

        let p1 = "jack".to_string();
        assert!(deck.add_player(&p1, 0, 100).is_ok());
        assert!(deck.start().is_err());

        let p2 = "jerry".to_string();
        assert!(deck.add_player(&p1, 1, 200).is_err());
        assert!(deck.add_player(&p2, 0, 200).is_err());
        assert!(deck.add_player(&p2, 1, 200).is_ok());

        assert!(deck.m_players[0].m_cards[0] == Card::empty());
        assert!(deck.m_players[0].m_cards[1] == Card::empty());

        assert!(deck.start().is_ok());

        assert!(deck.m_executor == 0);
        assert!(deck.m_button == 1);
        assert!(deck.m_sb == 0);
        assert!(deck.m_bb == 1);

        assert!(deck.m_players[0].m_cards[0] != Card::empty());
        assert!(deck.m_players[0].m_cards[1] != Card::empty());
    }
}