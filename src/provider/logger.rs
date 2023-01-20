use crate::game::log::DeckLogger;

struct Logger {

}

impl DeckLogger for Logger {
    fn publish_deck_state(&mut self, state: &crate::game::log::DeckState) {
        todo!()
    }

    fn publish_private_player_state(&mut self, state: &crate::game::log::PrivatePlayerState) {
        todo!()
    }

    fn publish_settle_state(&mut self, state: &crate::game::log::SettleState) {
        todo!()
    }
}