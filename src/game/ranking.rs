use super::card::Card;
use itertools::Itertools;

#[derive(Clone, PartialEq, Eq, PartialOrd, Ord)]
pub enum CardType {
    HighCard(i32, i32, i32, i32, i32),
    Pair(i32, i32, i32, i32),
    TwoPair(i32, i32, i32),
    Set(i32, i32, i32),
    Straight(i32),
    Flush(i32, i32, i32, i32, i32),
    FullHouse(i32, i32),
    FourOfAKind(i32, i32),
    StraightFlush(i32),
}

#[derive(Clone, PartialEq, Eq)]
pub struct CardScore {
    pub m_type: CardType,
    pub m_cards: Vec<Card>
}

pub fn calculate_player_hand_score(hand: &Vec<Card>, public: &Vec<Card>) -> CardScore {
    let mut cards = hand.clone();
    cards.extend(public);

    let mut max_score: Option<CardScore> = None;

    for combination in cards.into_iter().combinations(5) {
        let score = calculate_cards_score(&combination);
        match max_score {
            Some(ref last_score) => {
                if score.m_type >= last_score.m_type {
                    max_score.replace(score);
                }
            }
            None => { max_score.replace(score); }
        }
    }

    return max_score.unwrap();
}

fn is_flush(cards: &Vec<Card>) -> Option<CardType> {
    if cards.windows(2).all(|c2| c2[0].m_color == c2[1].m_color) {
        return Some(CardType::Flush(cards[4].m_num, cards[3].m_num, cards[2].m_num, cards[1].m_num, cards[0].m_num));
    }
    return None;
}

fn is_straight(cards: &Vec<Card>) -> Option<CardType> {
    let head = cards[4];
    for i in 0..4 {
        if cards[i].m_num + 1 != cards[i+1].m_num {
            return None;
        }
    }

    if cards[3].m_num == 4 && head.m_num == 14 {
        return Some(CardType::Straight(5));
    }

    if cards[3].m_num + 1 == head.m_num {
        return Some(CardType::Straight(head.m_num));
    }

    return None;
}

fn is_four_of_a_kind(cards: &Vec<Card>) -> Option<CardType> {
    let pos = cards.windows(4).position(|c4| c4.iter().all(|c| c.m_num == c4[0].m_num))?;
    return Some(CardType::FourOfAKind(cards[pos].m_num, cards[(pos + 4) % 5].m_num));
}

fn is_full_house(cards: &Vec<Card>) -> Option<CardType> {
    if let Some(CardType::Set(set_card, high, low)) = is_set(cards) {
        if high == low {
            return Some(CardType::FullHouse(set_card, high));
        }
    }
    return None;
}

fn is_set(cards: &Vec<Card>) -> Option<CardType> {
    let pos = cards.windows(3).position(|c3| c3.iter().all(|c| c.m_num == c3[0].m_num))?;
    let mut rest_cards = vec![cards[(pos+3) % 5].m_num, cards[(pos+4) % 5].m_num];
    rest_cards.sort();
    return Some(CardType::Set(cards[pos].m_num, rest_cards[1], rest_cards[0]));
}

fn is_two_pair(cards: &Vec<Card>) -> Option<CardType> {
    let pos = cards.windows(2).position(|c2| c2[0].m_num == c2[1].m_num)?;
    let pos_second = cards[pos+2..].windows(2).position(|c2| c2[0].m_num == c2[1].m_num)? + pos + 2;
    let min_num = cards[pos].m_num;
    let max_num = cards[pos_second].m_num;
    if min_num != max_num {
        let single_card_pos = cards.into_iter().position(|c| c.m_num != max_num && c.m_num != min_num)?;
        return Some(CardType::TwoPair(max_num, min_num, cards[single_card_pos].m_num))
    }
    return None;
}

fn is_pair(cards: &Vec<Card>) -> Option<CardType> {
    let mut reversed_cards = cards.clone();
    reversed_cards.reverse();

    let pair_cards_pos = cards.windows(2).position(|c2| c2[0].m_num == c2[1].m_num)?;
    if cards[pair_cards_pos..].windows(2).any(|c2| c2[0].m_num == c2[1].m_num) {
        return None;
    }
    let pair_num = cards[pair_cards_pos].m_num;
    let mut rest_cards = cards.iter().filter(|c| c.m_num != pair_num).collect_vec();
    rest_cards.sort();
    return Some(CardType::Pair(pair_num, rest_cards[2].m_num, rest_cards[1].m_num, rest_cards[0].m_num));
}

fn calculate_cards_score(cards: &Vec<Card>) -> CardScore {
    let mut cards = cards.clone();
    cards.sort_unstable();

    if let Some(flush) = is_flush(&cards) {
        if let Some(CardType::Straight(head)) = is_straight(&cards) {
            return CardScore{m_type: CardType::StraightFlush(head), m_cards: cards}; 
        }
        return CardScore{m_type: flush, m_cards: cards};
    }

    if let Some(straight) = is_straight(&cards) {
        return CardScore{m_type: straight, m_cards: cards};
    }

    if let Some(tomb) = is_four_of_a_kind(&cards) {
        return CardScore{m_type: tomb, m_cards: cards};
    }

    if let Some(fullhouse) = is_full_house(&cards) {
        return CardScore{m_type: fullhouse, m_cards: cards};
    }

    if let Some(set) = is_set(&cards) {
        return CardScore{m_type: set, m_cards: cards};
    }

    if let Some(two_pair) = is_two_pair(&cards) {
        return CardScore{m_type: two_pair, m_cards: cards};
    }

    if let Some(pair) = is_pair(&cards) {
        return CardScore{m_type: pair, m_cards: cards};
    }

    return CardScore{m_type: CardType::HighCard(cards[4].m_num, cards[3].m_num, cards[2].m_num, cards[1].m_num, cards[0].m_num), m_cards: cards}

}

#[cfg(test)]
mod tests {
    use super::CardType;

    #[test]
    fn card_type_cmp() {
        let high_card = CardType::HighCard(3, 0, 0, 2, 0);
        let high_card_2 = CardType::HighCard(2, 0, 0, 4, 0);

        assert!(high_card > high_card_2);

        let flush = CardType::Flush(10, 9, 8, 4, 3);

        assert!(flush > high_card);
    }
}