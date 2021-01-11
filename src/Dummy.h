#ifndef SAILGAME_TEXAS
#define SAILGAME_TEXAS

#include <map>
#include <string>
#include <vector>

#include "defines.h"

class DummyBackdoor;

class Dummy {
public:
  enum { STOP = 0, READY, NOT_YOUR_TURN, INVALID_BET, INVALID_PLAYER_NUM };

  friend class DummyBackdoor;

private:
  std::map<texas_defines::uid_t, texas_defines::PlayerStat> players;

  std::vector<texas_defines::card_t> board, deck;
  std::vector<texas_defines::uid_t> winners;

  texas_defines::status_t state;
  int user_count, alive_count;
  texas_defines::uid_t next_pos, button, small_blind, last_raised;
  texas_defines::chip_t cur_chips;
  bool raised, round_ends;

public:
  explicit Dummy()
      : state(STOP), user_count(0), next_pos(0), button(0), small_blind(0),
        cur_chips(0), raised(false), round_ends(false) {}

  const texas_defines::status_t Begin();
  const texas_defines::uid_t Join(const std::string &addr);
  // positive -> raise or call; zero -> check; -1 -> conceed.
  const texas_defines::status_t Play(const texas_defines::uid_t uid,
                                     const texas_defines::chip_t value);
  const texas_defines::chip_t Topup(const texas_defines::uid_t uid,
                                    const texas_defines::chip_t value) {
    return (players.at(uid).bankroll += value);
  }

  const texas_defines::GameStatus
  DumpStatusForUser(const texas_defines::uid_t uid) const;

  const texas_defines::uid_t FirstPlayer() const { return 1; }
  const texas_defines::uid_t LastPlayer() const { return user_count; }

private:
  const texas_defines::score_t Score(const texas_defines::uid_t uid);
  void Evaluate();
  void ResetGame();
  void NextCard(texas_defines::uid_t uid);
  const texas_defines::uid_t NextPlayer(texas_defines::uid_t uid, bool alive);
  void Shuffle();
};

namespace poker {

// clang-format off
  enum {
    H1 = 0x11, H2, H3, H4, H5, H6, H7, H8, H9, H10, HJ, HQ, HK, // Hearts
    S1 = 0x21, S2, S3, S4, S5, S6, S7, S8, S9, S10, SJ, SQ, SK, // Spades
    D1 = 0x31, D2, D3, D4, D5, D6, D7, D8, D9, D10, DJ, DQ, DK, // Diamonds
    C1 = 0x41, C2, C3, C4, C5, C6, C7, C8, C9, C10, CJ, CQ, CK  // Clubs
  };
// clang-format on

}; // namespace poker

#endif
