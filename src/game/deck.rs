use crate::game::card::Card;
use crate::game::player::Player;
use crate::game::player::PlayerState;
use crate::game::card::Dealer;

#[derive(Copy, Clone)]
pub enum PlayerAction {
    CHECK,
    BET(i32),
    FOLD,
    SHOWHAND
}

pub struct DeckConfig {
    m_small_blind: i32,
    m_big_blind: i32,
    m_timeout_secs: i32
}

impl DeckConfig {
    pub fn default() -> DeckConfig {
        return DeckConfig {m_small_blind: 1, m_big_blind: 2, m_timeout_secs: 30}
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
}

impl Deck {
    pub fn new(max_players: usize) -> Deck {
        let mut deck = Deck{m_config: DeckConfig::default(), m_players: vec![Player::empty(); max_players], m_cards: Vec::new(), m_dealer: Dealer::new(),
            m_button: 0, m_sb: 0, m_bb: 0, m_executor: 0};

        return deck
    }

    pub fn update_config(&self, config: &DeckConfig) -> Result<String, String>
    {
        return Ok("".into())
    }

    pub fn update_player(&self, pos: usize, name: String, chip: i32, status: PlayerState) -> Result<String, String> {
        return Ok("".into())
    }

    pub fn start(&mut self) -> Result<String, String> {
        let mut available_players : i32 = 0;
        for p in &self.m_players {
            match p.m_state  {
                PlayerState::ACTIVE => available_players += 1,
                PlayerState::WAITING => available_players += 1,
                _ => (),
            }
        }
        if available_players < 2 {
            return Err("".into())
        }

        for p in &mut self.m_players {
            match p.m_state  {
                PlayerState::ACTIVE => p.m_state = PlayerState::PLAYING,
                PlayerState::WAITING => p.m_state = PlayerState::PLAYING, // big blind
                _ => (),
            }
        }

        self.init_button_and_blind();
        self.init_cards();
        self.reset_timer();

        return Ok("".into())
    }

    pub fn stop(&self) -> Result<String, String> {
        return Ok("".into())
    }

    pub fn act(&self, pos: usize, name: String, action: PlayerAction) -> Result<String, String> {
        return Ok("".into())
    }

    fn init_button_and_blind(&mut self) {

    }
    fn init_cards(&mut self) {

    }
    fn reset_timer(&mut self) {

    }
    fn timeout(&mut self) 
    {

    }
    fn reset(&mut self) {

    }

}