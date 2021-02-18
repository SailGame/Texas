#pragma once

#include <map>
#include <string>
#include <vector>

#include <sailgame/common/state_machine.h>

#include "defines.h"

namespace SailGame {
namespace Texas {

class DummyBackdoor;

class Dummy {
public:
  enum GAME_STATE { STOP = 0, PLAYING, INVALID_PLAYER_NUM };
  enum PLAY_STATE { OK = 0, NOT_PLAYING, NOT_YOUR_TURN, INVALID_BET };
  friend class DummyBackdoor;

private:
  std::map<texas_defines::uid_t, texas_defines::PlayerStat> players;

  std::vector<texas_defines::card_t> board, deck;
  std::vector<texas_defines::uid_t> winners;

  GAME_STATE state;
  int user_count, alive_count;
  texas_defines::uid_t next_pos, button, small_blind, last_raised;
  texas_defines::chip_t cur_chips;
  bool raised, round_ends;

public:
  explicit Dummy()
      : state(STOP), user_count(0), next_pos(0), button(0), small_blind(0),
        cur_chips(0), raised(false), round_ends(false) {}

  const GAME_STATE Begin();
  const texas_defines::uid_t Join(const std::string &addr);
  // positive -> raise or call; zero -> check; -1 -> conceed.
  // return : true -> ok, false -> invalid operation, currently, we don't
  // present the failure reason
  const PLAY_STATE Play(const texas_defines::uid_t uid,
                        const texas_defines::chip_t value);
  const texas_defines::chip_t Topup(const texas_defines::uid_t uid,
                                    const texas_defines::chip_t value) {
    return (players.at(uid).bankroll += value);
  }

  const GAME_STATE GameState() { return state; }

  const texas_defines::uid_t FirstPlayer() const { return 1; }
  const texas_defines::uid_t LastPlayer() const { return user_count; }

private:
  const texas_defines::score_t Score(const texas_defines::uid_t uid);
  void Evaluate();
  void ResetGame();
  void NextCard(texas_defines::uid_t uid);
  const texas_defines::uid_t NextPlayer(texas_defines::uid_t uid, bool alive);
  void Shuffle();
  void Liquidate();
};

namespace poker {

// clang-format off
  enum {
    S1 = 0x01, S2, S3, S4, S5, S6, S7, S8, S9, S10, SJ, SQ, SK, // Spades
    H1 = 0x11, H2, H3, H4, H5, H6, H7, H8, H9, H10, HJ, HQ, HK, // Hearts
    C1 = 0x21, C2, C3, C4, C5, C6, C7, C8, C9, C10, CJ, CQ, CK,  // Clubs
    D1 = 0x31, D2, D3, D4, D5, D6, D7, D8, D9, D10, DJ, DQ, DK // Diamonds
  };
// clang-format on

inline int GetCardColor(int card) { return (card >> 4) & 0xf; };
inline int GetCardNum(int card) { return card & 0xf; };

}; // namespace poker

} // namespace Texas
} // namespace SailGame
