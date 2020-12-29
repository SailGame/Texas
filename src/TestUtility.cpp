#include "TestUtility.h"

#include <iostream>

void is_equal(int src, int target, int linum) {
  if (src == target)
    return;
  std::cerr << "At line: " << linum << ", expect [" << target << "(0x"
            << std::hex << target << ")], but got [" << std::dec << src << "(0x"
            << std::hex << src << ")]" << std::endl;
  exit(1);
}
