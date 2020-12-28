#include "Dummy.h"
#include <algorithm>
#include <random>

const Dummy::uid_t Dummy::Join(const std::string &addr) {
  ++user_count;
  uid2addr.emplace(LastPlayer(), addr);
  holecards.emplace(LastPlayer(), std::vector<card_t>());
  return LastPlayer();
}

const Dummy::status_t Dummy::Play(const uid_t uid, const chip_t bet) {
  if (state != READY)
    return state;

  if (uid != prev_pos + 1)
    // Not the turn for the user of @uid.
    return NOT_YOUR_TURN;

  if (bet < cur_chips && bet != -1)
    // Invalid chip @bet given.
    return INVALID_BET;

  if (bet > cur_chips) {
    // Raise
    raised = true;
    cur_chips = bet;
  }
  roundbets[uid] = bet;

  if (uid == cutoff) {
    if (raised) {
      // Someone raised within this turn.
      raised = false;
    } else {
      // All checked.
      if (board.size() < 3) {
        // Flop.
        NextCard(-1);
        NextCard(-1);
        NextCard(-1);

      } else if (board.size() < 5) {
        // Turn or river.
        NextCard(-1);
      } else {
        // Evaluate the winner.
        prev_winner = Evaluate();

        // Liquidation
        chip_t total_chips = 0;
        for(uid_t uid = FirstPlayer(); uid <= LastPlayer(); ++uid) {
          total_chips += roundbets[uid];
          bankroll[uid] -= roundbets[uid];
        }
        bankroll[uid] += total_chips;

        return state;
      }
    }
  }

  if (++prev_pos == LastPlayer())
    prev_pos = FirstPlayer() - 1;

  return state;
}

const GameStatus Dummy::DumpStatusForUser(const uid_t uid) const {
  GameStatus ret;
  ret.chips = chips;
  ret.board = board;
  ret.personal = holecards.at(uid);
  return ret;
}

const Dummy::status_t Dummy::Begin() {
  // Start a game turn from initialized status or return state.

  if (state == STOP)
    ResetGame();

  return state;
}

const Dummy::uid_t Dummy::Evaluate() {
  // Evaluate the game to determine the winner.

  if (board.size() != 5)
    return 0;

  uid_t winner = 0;
  int top_score = 0;
  for (uid_t uid = FirstPlayer(); uid <= LastPlayer(); ++uid) {
    int cur_score = Score(uid);
    if (cur_score > top_score) {
      top_score = cur_score;
      winner = uid;
    }
  }
  state = STOP;
  return winner;
}

void Dummy::ResetGame() {
  // *Not* to initialize the whole game engine.

  // 1. reset game status;
  chips.resize(0);
  for (auto &e : holecards)
    e.second.resize(0);
  for (auto &e : roundbets)
    e.second = 0;
  board.resize(0);

  state = READY;
  if (++button > LastPlayer())
    button = 1;
  prev_pos = button;
  cutoff = prev_pos + 1;
  if (cutoff > LastPlayer())
    cutoff = 1;
  cur_chips = 0;

  // 2. shuffle;
  Shuffle();

  // 3. Deal cards;
  for (uid_t uid = FirstPlayer(); uid <= LastPlayer(); ++uid) {
    NextCard(uid);
    NextCard(uid);
  }

  // 4. Preflop
  prev_pos = cutoff - 1;
  Play(prev_pos + 1 > LastPlayer() ? 1 : prev_pos + 1, 1);
  Play(prev_pos + 1 > LastPlayer() ? 1 : prev_pos + 1, 2);
  raised = false;
}

void Dummy::NextCard(uid_t uid) {
  // Deliver a new card from deck.
  // @uid = -1 => deal to the board;
  // @uid > 0 => deal to certain player.

  const card_t card = deck.back();
  deck.pop_back();

  if (uid == -1) {
    board.push_back(card);
  } else if (uid > 0) {
    holecards[uid].push_back(card);
  } else {
    exit(-1);
  }
}

void Dummy::Shuffle() {
  deck.resize(0);
  for (int card = poker::H1; card <= poker::HK; ++card)
    deck.push_back(card);
  for (int card = poker::S1; card <= poker::SK; ++card)
    deck.push_back(card);
  for (int card = poker::D1; card <= poker::DK; ++card)
    deck.push_back(card);
  for (int card = poker::C1; card <= poker::CK; ++card)
    deck.push_back(card);

  auto rng = std::default_random_engine{};
  std::shuffle(deck.begin(), deck.end(), rng);
}

const void Dummy::DumpDebugMessages(std::ostream &out) {
  using namespace std;
  const string divider(30, '=');
  out << divider << endl;
  out << "State: " << state << "\t\tThe number of players: " << user_count
      << "\t\tBoard: " << std::hex;
  for (auto const card : board)
    out << card << ' ';
  out << std::dec << '[' << board.size() << ']' << endl;
  out << "Deck: " << std::hex;
  for (auto const card : deck)
    out << card << ' ';
  out << std::dec << '[' << deck.size() << ']' << endl;
  out << "Previous pos: " << prev_pos << "\t\tButton pos: " << button
      << "\t\tCutoff: " << cutoff << "\t\tChips: " << cur_chips << endl;
  out << "Someone raised this turn? " << std::boolalpha << raised << std::dec
      << endl;
  for (uid_t uid = 1; uid <= LastPlayer(); ++uid) {
    out << endl;
    out << "\t[" << uid2addr[uid] << "]\tuid: " << uid << endl;
    out << "\tholecards: " << std::hex;
    for (auto const card : holecards[uid])
      out << card << ' ';
    out << std::dec << endl;
    out << "\tbankroll: " << bankroll[uid] << endl;
    out << "\troundbets: " << roundbets[uid] << endl;
  }
  out << divider << endl;
}
