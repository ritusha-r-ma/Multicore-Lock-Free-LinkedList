CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++2a -g -O3

SRC = test.cpp
EXEC = test

all: $(EXEC)

$(EXEC): $(SRC) lockFreeLinkedListWithCheckPoint.h
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(SRC) -lpthread

reclaimer.h:
	git submodule update --remote --recursive --init

.PHONY: clean

clean:
	rm -rf $(EXEC)
