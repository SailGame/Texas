use core::slice::Iter;
use rand::seq::SliceRandom;
use rand::thread_rng;

#[derive(Copy, Clone)]
pub enum CARD_COLOR {
    SPADE,
    HEART,
    CLUB,
    DIAMOND,
    NONE,
}

impl CARD_COLOR {
    pub fn iterator() -> Iter<'static, CARD_COLOR> {
        static CARD_COLORS: [CARD_COLOR; 4] = [
            CARD_COLOR::SPADE,
            CARD_COLOR::HEART,
            CARD_COLOR::CLUB,
            CARD_COLOR::DIAMOND,
        ];
        CARD_COLORS.iter()
    }
}

#[derive(Copy, Clone)]
pub struct Card {
    m_color: CARD_COLOR,
    m_num: i32, // 2 - 14
}

impl Card {
    pub fn empty() -> Card {
        return Card {
            m_color: CARD_COLOR::NONE,
            m_num: 0,
        };
    }

    pub fn new(color: CARD_COLOR, num: i32) -> Card {
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

        for color in CARD_COLOR::iterator() {
            for num in 2..14 {
                dealer.m_cards.push(Card {
                    m_color: color.clone(),
                    m_num: num,
                });
            }
        }
        return dealer;
    }

    pub fn shuffle(d: &mut Self) {
        d.m_cards.shuffle(&mut thread_rng());
        d.m_cursor = 0;
    }

    pub fn eject_card(d: &mut Self, burn: bool) -> Card {
        let cursor = d.m_cursor;
        d.m_cursor += 1;
        if burn {
            d.m_cursor += 1
        }
        return d.m_cards[cursor];
    }
}
