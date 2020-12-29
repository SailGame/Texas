#include <iostream>

#include "Dummy.h"

namespace {
Dummy dm;
bool logflag = 0;
void print_log();
} // namespace

int main(int argc, char **argv) {
  if (argc == 2 && std::string(argv[1]) == "-m")
    logflag = 1;

  print_log();

  dm.Join("a");
  dm.Join("b");
  dm.Join("c");
  dm.Join("d");
  print_log();

  dm.Begin();
  print_log();

  dm.Play(3, 2);
  dm.Play(4, 2);
  print_log();

  // Cutoff calls and flop.
  dm.Play(1, 2);
  print_log();

  // All check in turn round.
  dm.Play(2, 2);
  dm.Play(3, 2);
  dm.Play(4, 2);
  dm.Play(1, 2);
  print_log();

  // All check in river round.
  dm.Play(2, 2);
  dm.Play(3, 2);
  dm.Play(4, 2);
  dm.Play(1, 2);
  print_log();

  return 0;
}

namespace {
void print_log() {
  if (!logflag)
    return;
  dm.DumpDebugMessages(std::cout);
}
} // namespace
