#ifndef SAILGAME_TEXAS
#define SAILGAME_TEXAS

#include <iosfwd>
#include <map>
#include <string>
#include <vector>

struct GameStatus;
struct CardScore;

class Dummy {
public:
  enum class GameSignal { STOP, READY, NOT_YOUR_TURN, INVALID_BET };

  using uid_t = int;
  using chip_t = int;
  using card_t = int;
  using status_t = GameSignal;
  using score_t = CardScore;

private:
  std::map<uid_t, const std::string> uid2addr;

  std::map<uid_t, std::vector<card_t>> holecards;
  std::map<uid_t, chip_t> bankroll, roundbets;
  std::map<uid_t, int> alive;
  std::vector<card_t> board, deck;

  status_t state;
  int user_count, alive_count;
  uid_t prev_pos, button, prev_winner, small_blind;
  chip_t cur_chips;
  bool raised;

public:
  explicit Dummy()
      : state(GameSignal::STOP), user_count(0), prev_pos(-1), button(-1),
        prev_winner(-1), small_blind(-1), cur_chips(-1), raised(false) {}

  const status_t Begin();
  const uid_t Join(const std::string &addr);
  // positive -> raise or call; zero -> check; -1 -> conceed.
  const status_t Play(const uid_t uid, const chip_t value);

  const GameStatus DumpStatusForUser(const uid_t uid) const;
  const void DumpDebugMessages(std::ostream &out);

  const uid_t PreviousWinner() const { return prev_winner; }
  const uid_t FirstPlayer() const { return 1; }
  const uid_t LastPlayer() const { return user_count; }

private:
  const score_t Score(const uid_t uid);
  void Evaluate();
  void ResetGame();
  void NextCard(uid_t uid);
  void Shuffle();
};

struct GameStatus {
  std::vector<Dummy::card_t> board, personal;
  Dummy::status_t state;
  explicit GameStatus(Dummy::status_t state) : state(state) {}
};

struct CardScore {
  int royal_straight_flush;
  int straight_flush;
  int four_of_a_kind;
  int full_house;
  int flush;
  int straight;
  int three_of_a_kind;
  int two_pair;
  int one_pair;
  int high_card;

  int Compare(const CardScore &rhs) const;
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
