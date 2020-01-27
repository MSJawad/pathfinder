CXX = g++
CXXFLAGS = -std=c++17 -Wall -MMD -Werror=vla
EXEC = pathfinder
OBJECTS = window.o maze.o cell.o main.o 

DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC} -L/usr/X11R6/lib -lX11

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${EXEC} ${DEPENDS}
