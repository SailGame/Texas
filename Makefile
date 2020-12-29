SCORE_TEST_TARGET=TexasScoreTest

all:
	$(CXX) --std=c++17 Dummy.cpp main.cpp TexasScore.cpp

test_score:
	$(CXX) --std=c++17 -o $(SCORE_TEST_TARGET) TexasScoreTest.cpp TexasScore.cpp
	./$(SCORE_TEST_TARGET)

