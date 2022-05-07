use crate::game::card::Card;

#[derive(Copy, Clone)]
pub enum PlayerGameState {
    PLAYING,
    ALLIN,
    FOLD,
}

#[derive(Copy, Clone)]
pub enum PlayerConnectionState {
    OK,
    DISCONNECTED,
}

#[derive(Copy, Clone)]
pub enum PlayerDeckState {
    ACTIVE,
    WAITING,
    QUITING,
    EMPTY,
}

#[derive(Copy, Clone)]
pub struct PlayerState {
    pub m_game: PlayerGameState,
    pub m_conn: PlayerConnectionState,
    pub m_deck: PlayerDeckState,
}

impl PlayerState {
    pub fn empty() -> PlayerState {
        return PlayerState {
            m_game: PlayerGameState::PLAYING,
            m_conn: PlayerConnectionState::DISCONNECTED,
            m_deck: PlayerDeckState::EMPTY,
        };
    }
}

#[derive(Clone)]
pub struct Player {
    pub m_name: String,
    pub m_chip: usize,
    pub m_bet_chip: usize,
    pub m_cards: Vec<Card>,
    pub m_state: PlayerState,
}

impl Player {
    pub fn empty() -> Player {
        return Player {
            m_name: "".to_string(),
            m_chip: 0,
            m_bet_chip: 0,
            m_cards: Vec::new(),
            m_state: PlayerState::empty(),
        };
    }

    pub fn new(name: String, chip: usize) -> Player {
        return Player {
            m_name: name,
            m_chip: chip,
            m_bet_chip: 0,
            m_cards: vec![Card::empty(); 2],
            m_state: PlayerState {
                m_game: PlayerGameState::PLAYING,
                m_conn: PlayerConnectionState::OK,
                m_deck: PlayerDeckState::ACTIVE,
            }
        };
    }
}
