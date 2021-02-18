#pragma once

#include "Dummy.h"

namespace SailGame {
namespace Texas {

class State : public Common::IState {
public:
  State() = default;
  std::map<int, Dummy> mRooms;
};
} // namespace Texas
} // namespace SailGame