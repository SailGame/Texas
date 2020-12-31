add_library(tcore OBJECT Dummy.cpp Dummy.h TexasScore.cpp TexasScore.h)

target_include_directories(tcore PUBLIC ..)
