CPP = g++
CPPFLAGS = -std=c++11 -Wall -Wshadow -Wextra -g
CC = $(CPP) $(CPPFLAGS)
HDRS = domain.h node.h cube.h gnuplot.h bspline.h linear-combination.h bspline-non-rect.h coord.h
OBJS = domain.o node.o cube.o gnuplot.o bspline.o linear-combination.o bspline-non-rect.o
PROGRAMS = draw generate render-bsplines render-bspline-sum render-non-rect-support
SDLFLAGS = `sdl-config --libs --cflags`

all: $(PROGRAMS)

draw: draw.cpp
	$(CC) -o $@ $^ $(SDLFLAGS)

generate: generate.cpp $(OBJS)
	$(CC) -o $@ $^

render-bsplines: render-bsplines.cpp $(OBJS)
	$(CC) -o $@ $^

render-bspline-sum: render-bspline-sum.cpp $(OBJS)
	$(CC) -o $@ $^

render-non-rect-support: render-non-rect-support.cpp $(OBJS)
	$(CC) -o $@ $^

%.o: %.cpp $(HDRS)
	$(CC) -c -o $@ $<


.PHONY: zip-pngs clean

zip-pngs:
	rm -f bspline-pngs.zip
	zip -r bspline-pngs.zip png/

clean:
	@rm -f $(PROGRAMS) *.o

