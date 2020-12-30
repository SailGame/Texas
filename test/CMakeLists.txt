aux_source_directory(. TEST_DIR)
aux_source_directory(../src TEST_DIR)

add_executable(texas_test ${TEST_DIR})
target_link_libraries(texas_test PRIVATE gtest)

include(GoogleTest)
gtest_add_tests(texas_test "" AUTO)
