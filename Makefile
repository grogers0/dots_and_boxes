CXXFLAGS=-O2 -g -Wall
LINKFLAGS=-lpthread

all: dots solver

OBJECTS = Board.o InputOutput.o BasicMoveDeciders.o

dots: ${OBJECTS} DotsDriver.o
	g++ ${CXXFLAGS} -o $@ $^ ${LINKFLAGS}

solver: ${OBJECTS} Solver.o
	g++ ${CXXFLAGS} -o $@ $^ ${LINKFLAGS}

%.o: %.cpp
	g++ ${CXXFLAGS} -c $<

clean:
	rm -f *.o dots solver *.gcda core*
