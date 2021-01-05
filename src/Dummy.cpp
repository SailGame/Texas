#include "Dummy.h"
#include <algorithm>
#include <random>

#define MAX_BOARD_SIZE 5
#define FLOP_BOARD_SIZE 3
#define MIN_ROUND_PLAYER_NUM 4

const texas_defines::uid_t Dummy::Join(const std::string &addr) {
  ++user_count;
  uid2addr.emplace(LastPlayer(), addr);
  holecards.emplace(LastPlayer(), std::vector<texas_defines::card_t>());
  bankroll.emplace(LastPlayer(), 0);
  roundbets.emplace(LastPlayer(), 0);
  alive.emplace(LastPlayer(), 0);
  allin.emplace(LastPlayer(), 0);
  return LastPlayer();
}

const texas_defines::status_t Dummy::Play(const texas_defines::uid_t uid,
                                          const texas_defines::chip_t bet) {
  if (state != READY)
    return state;

  if (uid != next_pos)
    // Not the turn for the user of @uid.
    return NOT_YOUR_TURN;

  if (bet < cur_chips && bet != -1)
    // Invalid chip @bet given.
    return INVALID_BET;

  if (bet == -1) {
    // Someone choose to drop.
    alive[uid] = 0;
    if (--alive_count == 1)
      Evaluate();
  }

  if (bet > cur_chips) {
    // Raise
    raised = true;
    cur_chips = bet;
  }
  roundbets[uid] = bet;
  if (bet == bankroll[uid]) {
    // Denotes an all-in
    alive[uid] = 0;
    allin[uid] = 1;
  }

  if (uid == small_blind) {
    if (raised) {
      // Someone raised within this turn.
      raised = false;
    } else {
      // All checked.
      if (board.size() < FLOP_BOARD_SIZE) {
        // Flop.
        NextCard(-1);
        NextCard(-1);
        NextCard(-1);

      } else if (board.size() < MAX_BOARD_SIZE) {
        // Turn or river.
        NextCard(-1);
      } else {
        // Evaluate the winner.
        Evaluate();
        return state;
      }
    }
  }

  if (next_pos = NextPlayer(next_pos, true); !next_pos)
    state = STOP;

  return state;
}

const texas_defines::GameStatus
Dummy::DumpStatusForUser(const texas_defines::uid_t uid) const {
  texas_defines::GameStatus ret(state);
  ret.board = board;
  ret.personal = holecards.at(uid);
  return ret;
}

const texas_defines::status_t Dummy::Begin() {
  // Start a game turn from initialized status or return state.

  if (user_count < MIN_ROUND_PLAYER_NUM)
    return INVALID_PLAYER_NUM;
  if (state == STOP)
    ResetGame();

  return state;
}

void Dummy::Evaluate() {
  // Evaluate the game to determine the winner.

  const auto len = board.size();
  assert(len == MAX_BOARD_SIZE);

  texas_defines::uid_t winner = 0;
  texas_defines::score_t top_score;
  for (texas_defines::uid_t uid = FirstPlayer(); uid <= LastPlayer(); ++uid) {
    if (!alive[uid] && !allin[uid])
      continue;
    texas_defines::score_t cur_score = Score(uid);
    if (cur_score.Compare(top_score) == 1) {
      top_score = cur_score;
      winner = uid;
    }
  }
  state = STOP;
  assert(winner);

  // Liquidation
  texas_defines::chip_t total_chips = 0;
  for (texas_defines::uid_t uid = FirstPlayer(); uid <= LastPlayer(); ++uid) {
    total_chips += roundbets[uid];
    bankroll[uid] -= roundbets[uid];
  }
  bankroll[winner] += total_chips;

  prev_winner = winner;
}

void Dummy::ResetGame() {
  // *Not* to initialize the whole game engine.

  // 1. reset game status;
  for (auto &e : holecards)
    e.second.resize(0);
  for (auto &e : roundbets)
    e.second = 0;
  board.resize(0);

  alive_count = 0;
  for (auto &ent : alive) {
    ent.second = (bankroll[ent.first] > 0);
    alive_count += ent.second;
  }
  if (alive_count < MIN_ROUND_PLAYER_NUM)
    return;

  button = NextPlayer(button, 1);
  assert(button);
  small_blind = button + 1;
  next_pos = small_blind;
  if (small_blind > LastPlayer())
    small_blind = 1;
  cur_chips = 0;

  for (auto &ent : allin)
    ent.second = 0;

  state = READY;

  // 2. shuffle;
  Shuffle();

  // 3. Deal cards;
  for (texas_defines::uid_t uid = FirstPlayer(); uid <= LastPlayer(); ++uid) {
    NextCard(uid);
    NextCard(uid);
  }

  // 4. Preflop
  Play(next_pos, 1);
  Play(next_pos, 2);
  raised = false;
}

void Dummy::NextCard(texas_defines::uid_t uid) {
  // Deliver a new card from deck.
  // @uid = -1 => deal to the board;
  // @uid > 0 => deal to certain player.

  const auto card = deck.back();
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

const texas_defines::uid_t Dummy::NextPlayer(texas_defines::uid_t uid,
                                             bool bAlive) const {
  // When @alive is true, return the next living player, or 0 for none alive.
  if (bAlive && alive_count == 0)
    return 0;
  const auto mask = !bAlive;
  int cnt = user_count;
  while (cnt--) {
    if (++uid > LastPlayer())
      uid = FirstPlayer();
    if (alive.at(uid) || mask)
      return uid;
  }
  // Should never reach this.
  assert(0);
}
