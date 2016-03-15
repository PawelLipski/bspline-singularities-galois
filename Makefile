CPP = g++
CPPFLAGS = -std=c++11 -Wall -Wshadow -Wextra -g
CC = $(CPP) $(CPPFLAGS)
HDRS = domain.h node.h cube.h coord.h
OBJS = domain.o node.o cube.o
PROGRAMS = draw generate gnuplot
SDLFLAGS = `sdl-config --libs --cflags`

all: $(PROGRAMS)

draw: draw.cpp
	$(CC) -o $@ $^ $(SDLFLAGS)

generate: generate.cpp $(OBJS)
	$(CC) -o $@ $^

gnuplot: gnuplot.cpp cube.o
	$(CC) -o $@ $^

%.o: %.cpp $(HDRS)
	$(CC) -c -o $@ $<

.PHONY: zip-pngs clean

zip-pngs:
	rm -f bspline-pngs.zip
	zip -r bspline-pngs.zip png/

clean:
	@rm -f $(PROGRAMS) *.o

