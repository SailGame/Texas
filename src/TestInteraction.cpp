#include "Backdoor.h"
#include "TestUtility.h"

int main() {
  Dummy dm;
  DummyBackdoor dbd;

  assert_equal(dbd.CountPlayers(dm), 0);

  assert_equal(dm.Join("a"), 1);
  assert_equal(dm.Join("b"), 2);
  assert_equal(dm.Join("c"), 3);
  assert_equal(dm.Join("d"), 4);

  assert_equal(dbd.CountPlayers(dm), 4);

  assert_equal(dm.Play(1, 0), Dummy::STOP);
  assert_equal(dm.Play(2, 0), Dummy::STOP);
  assert_equal(dm.Play(3, 0), Dummy::STOP);
  assert_equal(dm.Play(4, 0), Dummy::STOP);

  assert_equal(dm.Begin(), Dummy::READY);

  assert_equal(dm.Play(1, 0), Dummy::INVALID_BET);
  assert_equal(dm.Play(2, 0), Dummy::NOT_YOUR_TURN);
  assert_equal(dm.Play(3, 0), Dummy::NOT_YOUR_TURN);
  assert_equal(dm.Play(4, 0), Dummy::NOT_YOUR_TURN);


  return 0;
}
