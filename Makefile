CPPFLAGS = --std=c++17
VPATH = src:./src

test_score: TexasScore.o TexasScoreTest.o
	$(CXX) -o $@ $^
	./$@

test_interaction: TexasScore.o InteractionTest.o Dummy.o
	$(CXX) -o $@ $^
	./$@
