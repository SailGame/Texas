use crate::game::card::Card;

#[derive(Copy, Clone)]
pub enum PlayerState {
    ACTIVE,
    WAITING,
    FOLD,
    DISCONNECTED,
    QUITING,
    EMPTY
}

#[derive(Clone)]
pub struct Player {
    m_name: String,
    m_chip: i32,
    m_cards: Vec<Card>,
    m_state: PlayerState
}

impl Player {
    pub fn empty() -> Player {
        return Player{m_name: "".to_string(), m_chip: 0, m_cards: Vec::new(), m_state: PlayerState::EMPTY}
    }

    pub fn new(name: String, chip: i32) -> Player
    {
        return Player{m_name: name, m_chip: chip, m_cards: vec![Card::empty(); 2], m_state: PlayerState::WAITING}
    }
}