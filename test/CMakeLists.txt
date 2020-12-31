project(texas_test)

aux_source_directory(. TEST_DIR)

add_executable(${PROJECT_NAME} ${TEST_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE gtest)
target_link_libraries(${PROJECT_NAME} PRIVATE tcore)

include(GoogleTest)
gtest_add_tests(${PROJECT_NAME} "" AUTO)
