#ifndef SAIL_GAME_TEXAS_TESTUTIL
#define SAIL_GAME_TEXAS_TESTUTIL

#define assert_equal(a, b) is_equal(a, b, __LINE__)
void is_equal(int src, int target, int linum);

#endif
