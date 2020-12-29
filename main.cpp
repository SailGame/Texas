#include <iostream>

#include "Dummy.h"

int main() {
  Dummy dm;
  dm.DumpDebugMessages(std::cout);

  dm.Join("a");
  dm.Join("b");
  dm.Join("c");
  dm.Join("d");
  dm.DumpDebugMessages(std::cout);

  dm.Begin();
  dm.DumpDebugMessages(std::cout);

  dm.Play(3, 2);
  dm.Play(4, 2);
  dm.DumpDebugMessages(std::cout);

  // Cutoff calls and flop.
  dm.Play(1, 2);
  dm.DumpDebugMessages(std::cout);

  // All check in turn round.
  dm.Play(2, 2);
  dm.Play(3, 2);
  dm.Play(4, 2);
  dm.Play(1, 2);
  dm.DumpDebugMessages(std::cout);

  // All check in river round.
  dm.Play(2, 2);
  dm.Play(3, 2);
  dm.Play(4, 2);
  dm.Play(1, 2);
  dm.DumpDebugMessages(std::cout);

  return 0;
}
