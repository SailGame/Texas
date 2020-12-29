CPPFLAGS = --std=c++17
VPATH = src:./src


all: test

test: test_score test_interaction

test_score: TexasScore.o TexasScoreTest.o TestUtility.o
	$(CXX) -o $@ $^
	./$@

test_interaction: TexasScore.o TestInteraction.o Dummy.o Backdoor.o TestUtility.o
	$(CXX) -o $@ $^
	./$@

.PHONY: clean
clean:
	rm *.o || rm ./test_* || true
