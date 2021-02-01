add_library(tcore OBJECT Dummy.cpp Dummy.h TexasScore.cpp TexasScore.h StateMachine.cpp StateMachine.h)

target_link_libraries(tcore PUBLIC texas_lib)

target_include_directories(tcore PUBLIC ..)

add_executable(Texas TexasServer.cpp)

target_link_libraries(Texas tcore)