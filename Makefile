CC = g++
DEPS = elo.hpp
CFLAGS = -c -Wall -std=c++11

all: elo

elo: elo.o
	$(CC) elo.o -o elo

elo.o: elo.cpp
	$(CC) $(CFLAGS) elo.cpp

clean:
	rm -f *.o elo
