CXXFLAGS=-O2 -g -Wall -std=c++0x
LINKFLAGS=-lpthread

all: dots solver brute_force

OBJECTS = Board.o InputOutput.o BasicMoveDeciders.o

dots: ${OBJECTS} DotsDriver.o
	g++ ${CXXFLAGS} -o $@ $^ ${LINKFLAGS}

solver: ${OBJECTS} Solver.o
	g++ ${CXXFLAGS} -o $@ $^ ${LINKFLAGS}

brute_force: ${OBJECTS} BruteForce.o
	g++ ${CXXFLAGS} -o $@ $^ ${LINKFLAGS}

%.o: %.cpp
	g++ ${CXXFLAGS} -c $<

clean:
	rm -f *.o dots solver *.gcda core*
