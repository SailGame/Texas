use core::slice::Iter;
use rand::seq::SliceRandom;
use rand::thread_rng;

#[derive(Copy, Clone, PartialEq, Eq)]
pub enum CardColor {
    SPADE,
    HEART,
    CLUB,
    DIAMOND,
    NONE,
}

impl CardColor {
    pub fn iterator() -> Iter<'static, CardColor> {
        static CARD_COLORS: [CardColor; 4] = [
            CardColor::SPADE,
            CardColor::HEART,
            CardColor::CLUB,
            CardColor::DIAMOND,
        ];
        CARD_COLORS.iter()
    }
}

#[derive(Copy, Clone, PartialEq, Eq)]
pub struct Card {
    m_color: CardColor,
    m_num: i32, // 2 - 14
}

impl Card {
    pub fn empty() -> Card {
        return Card {
            m_color: CardColor::NONE,
            m_num: 0,
        };
    }

    pub fn new(color: CardColor, num: i32) -> Card {
        return Card {
            m_color: color,
            m_num: num,
        };
    }
}

pub struct Dealer {
    m_cards: Vec<Card>,
    m_cursor: usize,
}

impl Dealer {
    pub fn new() -> Dealer {
        let mut dealer = Dealer {
            m_cards: Vec::new(),
            m_cursor: 0,
        };

        for color in CardColor::iterator() {
            for num in 2..14 {
                dealer.m_cards.push(Card {
                    m_color: color.clone(),
                    m_num: num,
                });
            }
        }
        return dealer;
    }

    pub fn shuffle(&mut self) {
        self.m_cards.shuffle(&mut thread_rng());
        self.m_cursor = 0;
    }

    pub fn eject_card(&mut self, burn: bool) -> Card {
        let cursor = self.m_cursor;
        self.m_cursor += 1;
        if burn {
            self.m_cursor += 1
        }
        return self.m_cards[cursor];
    }
}
