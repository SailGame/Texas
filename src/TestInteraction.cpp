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

  return 0;
}
