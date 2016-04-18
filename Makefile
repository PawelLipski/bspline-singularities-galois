CPP = g++
CPPFLAGS = -std=c++11 -Wall -Wshadow -Wextra -g
CC = $(CPP) $(CPPFLAGS)
HDRS = domain.h node.h cube.h gnuplot.h bspline.h coord.h linearCombination.h bsplineNonRect.h
PROGRAMS = draw generate non-rect-support render-bsplines
SDLFLAGS = `sdl-config --libs --cflags`

all: $(PROGRAMS)

draw: draw.cpp
	$(CC) -o $@ $^ $(SDLFLAGS)

generate: generate.cpp domain.o node.o bspline.o gnuplot.o cube.o bsplineNonRect.o
	$(CC) -o $@ $^

non-rect-support: non-rect-support.cpp bspline.o gnuplot.o cube.o linearCombination.o bsplineNonRect.o
	$(CC) -o $@ $^

render-bsplines: render-bsplines.cpp bspline.o gnuplot.o cube.o
	$(CC) -o $@ $^

%.o: %.cpp $(HDRS)
	$(CC) -c -o $@ $<


.PHONY: zip-pngs clean

zip-pngs:
	rm -f bspline-pngs.zip
	zip -r bspline-pngs.zip png/

clean:
	@rm -f $(PROGRAMS) *.o

