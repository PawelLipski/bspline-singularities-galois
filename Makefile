CPP = g++
CPPFLAGS = -std=c++11 -Wall -Wshadow -Wextra -g
CC = $(CPP) $(CPPFLAGS)
HDRS = domain.h node.h cube.h gnuplot.h bspline.h coord.h linear-combination.h bspline-non-rect.h
PROGRAMS = draw generate render-bsplines render-bspline-sum render-non-rect-support
SDLFLAGS = `sdl-config --libs --cflags`

all: $(PROGRAMS)

draw: draw.cpp
	$(CC) -o $@ $^ $(SDLFLAGS)

generate: generate.cpp domain.o node.o bspline.o gnuplot.o cube.o bspline-non-rect.o
	$(CC) -o $@ $^

render-bsplines: render-bsplines.cpp bspline.o gnuplot.o cube.o
	$(CC) -o $@ $^

render-bspline-sum: render-bspline-sum.cpp bspline.o gnuplot.o cube.o linear-combination.o
	$(CC) -o $@ $^

render-non-rect-support: render-non-rect-support.cpp bspline.o gnuplot.o cube.o linear-combination.o bspline-non-rect.o
	$(CC) -o $@ $^

%.o: %.cpp $(HDRS)
	$(CC) -c -o $@ $<


.PHONY: zip-pngs clean

zip-pngs:
	rm -f bspline-pngs.zip
	zip -r bspline-pngs.zip png/

clean:
	@rm -f $(PROGRAMS) *.o

