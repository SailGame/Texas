use crate::game::card::Card;
use crate::game::player::Player;
use crate::game::card::Dealer;

pub struct Deck {
    m_players: Vec<Player>,
    m_cards: Vec<Card>,
    m_dealer: Dealer

}

impl Deck {
    pub fn new(max_players: usize) -> Deck {
        let mut deck = Deck{m_players: vec![Player::empty(); max_players], m_cards: Vec::new(), m_dealer: Dealer::new()};

        return deck
    }
}